#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/CFG.h"
#include "llvm/ADT/APInt.h"
#include "llvm/Support/raw_ostream.h"
#include "DataTable.h"

using namespace llvm;

class IRGenVisitor : public ASTvisitor {

    std::unique_ptr<LLVMContext> TheContext = std::make_unique<LLVMContext>();
    std::unique_ptr<Module> TheModule = std::make_unique<Module>("MiniC IR", *TheContext);
    std::unique_ptr<IRBuilder<>> Builder = std::make_unique<IRBuilder<>>(*TheContext);
    DataTable* datTab = new DataTable();
    vector<BasicBlock*> loopBBs, afterLoopBBs;
    vector<AstNode*> loopNodes;

    Type* mapToLLVMType(string type) {
        if(type == "int" || type == "uint") {
            return Type::getInt32Ty(*TheContext);
        }
        else if(type == "char") {
            return Type::getInt8Ty(*TheContext);
        }
        else if(type == "bool") {
            return Type::getInt1Ty(*TheContext);
        }
        else{
            cout << "ERROR: No type mapping exists for: " << type << endl;
        }
    }

    Value* getAddrWithOffset(IdentifierAstNode* node) {
        if(node->getRowExpr()==NULL) {
            return datTab->getAddr(node->getID());
        }
        else{
            Value *zero = ConstantInt::get(*TheContext, APInt(32, "0", 10));;
            Value *ind = Builder->CreateGEP(datTab->getAddr(node->getID()), {zero, node->getRowExpr()->accept(this)});
            if(node->getColExpr()!=NULL) {
                ind = Builder->CreateGEP(ind, {zero, node->getColExpr()->accept(this)});
            }
            return ind;
        }
    }

    antlrcpp::Any visit(ProgAstNode *node) override {

        // Creating scanf and printf
        vector<Type*> runTimeFuncArgs = {Type::getInt8PtrTy(*TheContext)};
        FunctionType *runTimeFuncType = FunctionType::get(Builder->getInt32Ty(), runTimeFuncArgs, true);
        Function::Create(runTimeFuncType, Function::ExternalLinkage, "printf", TheModule.get());
        Function::Create(runTimeFuncType, Function::ExternalLinkage, "scanf", TheModule.get());

        // Visiting the program
        for(auto declaration : node->getDeclList()) {
            declaration->accept(this);
        }

        // Correcting the basic blocks by removing instructions that may never be executed
        for(auto itr1=TheModule->getFunctionList().begin();itr1!=TheModule->getFunctionList().end();itr1++){
            Function &f = *itr1;
            for(auto itr2=f.getBasicBlockList().begin();itr2!=f.getBasicBlockList().end(); itr2++) {
                BasicBlock &bb = *itr2;

                bool outOfBlock = false;
                for(auto itr3=bb.begin();itr3!=bb.end();) {
                    Instruction &i = *itr3;
                    itr3++;

                    if(outOfBlock) {
                        i.eraseFromParent();
                    }
                    if(string(i.getOpcodeName()) == "ret" || string(i.getOpcodeName()) == "br") {
                        outOfBlock = true;
                    }
                }
            }
        }

        // Dumping the IR to a string
        std::string IR;
        raw_string_ostream OS(IR);
        OS << *TheModule;
        OS.flush();
    
        return IR;
    }

    antlrcpp::Any visit(FunctionDeclAstNode *node) override {
        cout << "Visiting function: " << node->getIdentifier() << endl;

        // Changing to function scope
        datTab->createChild(node->getIdentifier());

        // Creating the parameters vector using LLVM types
        vector<Type*> params;
        for(auto p : node->getParamsList()) {
            params.push_back(mapToLLVMType(p->getType()));
        }

        // Creating the function
        FunctionType *Ftype = FunctionType::get(mapToLLVMType(node->getType()), params, false);
        Function *F = Function::Create(Ftype, Function::ExternalLinkage, node->getIdentifier(), TheModule.get());

        // Creating the one basic block
        BasicBlock *B0 = BasicBlock::Create(*TheContext, "entryBlock", F);
        Builder->SetInsertPoint(B0);

        // Adding argument names and creating local stack variables
        int i = 0;
        for(auto &Arg : F->args()) {
            Arg.setName(node->getParamsList()[i++]->getIdentifier()->getID());
            Value *addr = Builder->CreateAlloca(Arg.getType(), nullptr, Arg.getName());
            Builder->CreateStore(&Arg, addr);
            datTab->addEntry(addr, Arg.getName());
        }

        // Generating code for the statements
        node->getStatementBlock()->accept(this);

        // Validity check
        verifyFunction(*F);

        // Getting back to global scope
        datTab->moveToPar();

        return 0;
    }

    antlrcpp::Any visit(VariableDeclAstNode *node) override {
        cout << "Visiting Variable declaration of " << node->getIdentifier()->getID() << endl;

        Type *type;
        if(node->getIdentifier()->getRowExpr()!=NULL) { // Array
            ConstantInt *nRowCI = dyn_cast<ConstantInt>((Value*)node->getIdentifier()->getRowExpr()->accept(this));
            long long nRow = nRowCI->getLimitedValue();
            type = ArrayType::get(mapToLLVMType(node->getType()), nRow);
            
            if(node->getIdentifier()->getColExpr()!=NULL) {
                ConstantInt *nColCI = dyn_cast<ConstantInt>((Value*)node->getIdentifier()->getColExpr()->accept(this));
                long long nCol = nColCI->getLimitedValue();
                type = ArrayType::get(type, nCol);
            }
        }
        else{ // Simple variable
            type = mapToLLVMType(node->getType());
        }

        Value *addr;
        if(datTab->isScopeGlobal()) { //Global scope
            TheModule->getOrInsertGlobal(node->getIdentifier()->getID(), type);
            addr = TheModule->getNamedGlobal(node->getIdentifier()->getID());
            dynamic_cast<GlobalVariable*>(addr)->setLinkage(GlobalValue::CommonLinkage);

            // Global variables need to be initialized
            ConstantAggregateZero* constZero = ConstantAggregateZero::get(type);
            dynamic_cast<GlobalVariable*>(addr)->setInitializer(constZero);
        }
        else{ // Local scope
            addr = Builder->CreateAlloca(type, nullptr, node->getIdentifier()->getID());
        }

        if(node->getIsInitialized()) {
            Builder->CreateStore(node->getAssignedValue()->accept(this), addr);   
        }

        datTab->addEntry(addr, node->getIdentifier()->getID());
        return 0;
    }

    antlrcpp::Any visit(AssignStatementAstNode *node) override {
        cout << "Visiting assignment of " << node->getIdentifier()->getID() << endl;

        if(node->getRhsExpr() != NULL) {
            Builder->CreateStore(node->getRhsExpr()->accept(this), getAddrWithOffset(node->getIdentifier()));
        }
        else{
            Value* idVal = node->getIdentifier()->accept(this);
            Value* one = ConstantInt::get(*TheContext, APInt(32, "1", 10));
            if(node->getUnaryOp() == "++"){
                Builder->CreateStore(Builder->CreateAdd(idVal, one, "inc"), getAddrWithOffset(node->getIdentifier()));
            }
            else{
                Builder->CreateStore(Builder->CreateSub(idVal, one, "dec"), getAddrWithOffset(node->getIdentifier()));
            }
        }
        return 0;
    }

    antlrcpp::Any visit(StatementBlockAstNode *node) override {
        for(auto stAndDecl : node->getStatementsAndDecls()) {
            stAndDecl->accept(this);
        }
        return 0;
    }

    antlrcpp::Any visit(ForStatementAstNode *node) override {
        cout << "Visiting a For loop\n";

        datTab->createChild(datTab->getCurrFunc());

        node->getInitialization()->accept(this);

        Function *TheF = Builder->GetInsertBlock()->getParent();
        BasicBlock *LoopBB = BasicBlock::Create(*TheContext, "loop", TheF);
        BasicBlock *AfterBB = BasicBlock::Create(*TheContext, "afterLoop", TheF);

        Value *condVal = node->getCondition()->accept(this);
        Builder->CreateCondBr(condVal, LoopBB, AfterBB);

        Builder->SetInsertPoint(LoopBB);
        loopBBs.push_back(LoopBB);
        afterLoopBBs.push_back(AfterBB);
        loopNodes.push_back(node);
        node->getStatementBlock()->accept(this);
        node->getStep()->accept(this);
        Value *condVal1 = node->getCondition()->accept(this);
        Builder->CreateCondBr(condVal1, LoopBB, AfterBB);

        loopBBs.pop_back();
        afterLoopBBs.pop_back();
        loopNodes.pop_back();
        Builder->SetInsertPoint(AfterBB);

        datTab->moveToPar();

        return 0;
    }

    antlrcpp::Any visit(WhileStatementAstNode *node) override {
        
        cout << "Visiting a While loop\n";

        datTab->createChild(datTab->getCurrFunc());
        
        Function *TheF = Builder->GetInsertBlock()->getParent();
        BasicBlock *LoopBB = BasicBlock::Create(*TheContext, "loop", TheF);
        BasicBlock *AfterBB = BasicBlock::Create(*TheContext, "afterLoop", TheF);

        Value *condVal = node->getCondition()->accept(this);
        Builder->CreateCondBr(condVal, LoopBB, AfterBB);
        
        Builder->SetInsertPoint(LoopBB);
        loopBBs.push_back(LoopBB);
        afterLoopBBs.push_back(AfterBB);
        loopNodes.push_back(node);
        node->getStatementBlock()->accept(this);
        Value *condVal1 = node->getCondition()->accept(this);
        Builder->CreateCondBr(condVal1, LoopBB, AfterBB);
        
        loopBBs.pop_back();
        afterLoopBBs.pop_back();
        loopNodes.pop_back();
        Builder->SetInsertPoint(AfterBB);
        
        datTab->moveToPar();

        return 0;
    }

    antlrcpp::Any visit(IfStatementAstNode *node) override {
        cout << "Visiting an If statement\n";

        datTab->createChild(datTab->getCurrFunc());

        Function *TheF = Builder->GetInsertBlock()->getParent();
        BasicBlock *IfBB = BasicBlock::Create(*TheContext, "if", TheF);
        BasicBlock *MergeBB = BasicBlock::Create(*TheContext, "merge", TheF);
        
        Value *condVal = node->getCondition()->accept(this);
        Builder->CreateCondBr(condVal, IfBB, MergeBB);
        
        Builder->SetInsertPoint(IfBB);
        node->getStatementBlock()->accept(this);
        Builder->CreateBr(MergeBB);

        Builder->SetInsertPoint(MergeBB);
        
        datTab->moveToPar();

        return 0;
    }

    antlrcpp::Any visit(IfElseStatementAstNode *node) override {
        cout << "Visiting an If-Else statement\n";

        datTab->createChild(datTab->getCurrFunc());

        Function *TheF = Builder->GetInsertBlock()->getParent();
        BasicBlock *IfBB = BasicBlock::Create(*TheContext, "if", TheF);
        BasicBlock *ElseBB = BasicBlock::Create(*TheContext, "else", TheF);
        BasicBlock *MergeBB = BasicBlock::Create(*TheContext, "merge", TheF);
        
        Value *condVal = node->getCondition()->accept(this);
        Builder->CreateCondBr(condVal, IfBB, ElseBB);
        
        Builder->SetInsertPoint(IfBB);
        node->getIfStatementBlock()->accept(this);
        Builder->CreateBr(MergeBB);

        Builder->SetInsertPoint(ElseBB);
        node->getElseStatementBlock()->accept(this);
        Builder->CreateBr(MergeBB);

        Builder->SetInsertPoint(MergeBB);

        datTab->moveToPar();

        return 0;
    }

    antlrcpp::Any visit(BreakStatementAstNode *node) override {
        Builder->CreateBr(afterLoopBBs.back());
        return 0;
    }

    antlrcpp::Any visit(ContinueStatementAstNode *node) override {
        
        ForStatementAstNode *forNode = dynamic_cast<ForStatementAstNode*>(loopNodes.back());
        if(forNode!=nullptr) {
            forNode->getStep()->accept(this);
            Value *condVal = forNode->getCondition()->accept(this);
            Builder->CreateCondBr(condVal, loopBBs.back(), afterLoopBBs.back());
        }
        else{
            WhileStatementAstNode *whileNode = dynamic_cast<WhileStatementAstNode*>(loopNodes.back());
            Value *condVal = whileNode->getCondition()->accept(this);
            Builder->CreateCondBr(condVal, loopBBs.back(), afterLoopBBs.back());
        }
        return 0;
    }

    antlrcpp::Any visit(ReturnStatementAstNode *node) override {
        Value* retVal = node->getExpr()->accept(this);
        Builder->CreateRet(retVal);
        return 0;
    }

    antlrcpp::Any visit(PrintStatementAstNode *node) override {
        Function *thePrintf = TheModule->getFunction("printf");
        
        string temp = "";
        for(auto p : node->getCallParams()) {
            if(p->getType()=="int"){
                temp.append("%d");
            }
            else if(p->getType()=="uint"){
                temp.append("%u");
            }
            else if(p->getType()=="char"){
                temp.append("%c");
            }
            else if(p->getType()=="bool"){
                temp.append("%u");
            }
            else if(p->getType()=="newLine"){
                temp.push_back('\n');
            }
        }

        Value *printfFormat = Builder->CreateGlobalStringPtr(temp);

        vector<Value *> printfArgs = {printfFormat};
        for(auto p : node->getCallParams()){
            printfArgs.push_back(p->accept(this));
        }

        Builder->CreateCall(thePrintf, printfArgs);
        
        return 0;
    }

    antlrcpp::Any visit(ReadStringStatementAstNode *node) override {
        Value *scanfFormat = Builder->CreateGlobalStringPtr("%s");
        vector<Value *> scanfArgs = {scanfFormat, getAddrWithOffset(node->getCharArrayId())};

        Function *theScanf = TheModule->getFunction("scanf");
        Builder->CreateCall(theScanf, scanfArgs);

        return 0;
    }

    antlrcpp::Any visit(CallExprAstNode *node) override {
        cout << "Visiting call of: " << node->getIdentifier() << endl;

        Function *CalleeF = TheModule->getFunction(node->getIdentifier());    
        vector<Value*> ArgsV;
        for(auto param : node->getCallParams()) {
            ArgsV.push_back(param->accept(this));
        }
        return (Value*)Builder->CreateCall(CalleeF, ArgsV, "callfn");;
    }

    antlrcpp::Any visit(UnaryOpExprAstNode *node) override {
        
        if(node->getExpr() != NULL) {
            Value *exprVal = node->getExpr()->accept(this);
            if(node->getUnaryOp() == "-") {
                return Builder->CreateNeg(exprVal, "negOp");
            }
            else if(node->getUnaryOp() == "!") {
                return Builder->CreateNot(exprVal, "notOp");
            }
            else { // Type Casting
                return (Value*)Builder->CreateIntCast(exprVal, mapToLLVMType(node->getUnaryOp()), false);
            }
        }
        else{
            Value *idVal = node->getIdentifier()->accept(this);
            Value* one = ConstantInt::get(*TheContext, APInt(32, "1", 10));
            if(node->getUnaryOp() == "++"){
                Builder->CreateStore(Builder->CreateAdd(idVal, one, "inc"), getAddrWithOffset(node->getIdentifier()));
            }
            else{
                Builder->CreateStore(Builder->CreateSub(idVal, one, "dec"), getAddrWithOffset(node->getIdentifier()));
            }
            return idVal;
        }
    }

    antlrcpp::Any visit(BinaryOpExprAstNode *node) override {
        
        Value *leftVal = node->getLeft()->accept(this);
        Value *rightVal = node->getRight()->accept(this);
        
        if(node->getBinaryOp() == "*") {
            return Builder->CreateMul(leftVal, rightVal, "mul");
        }
        else if(node->getBinaryOp() == "/") {
            if(node->getLeft()->getType()=="int"){
                return Builder->CreateSDiv(leftVal, rightVal, "sdiv");
            }
            else{
                return Builder->CreateUDiv(leftVal, rightVal, "udiv");
            }
        }
        else if(node->getBinaryOp() == "%") {
            if(node->getLeft()->getType()=="int"){
                return Builder->CreateSRem(leftVal, rightVal, "smod");
            }
            else{
                return Builder->CreateURem(leftVal, rightVal, "umod");
            }
        }
        else if(node->getBinaryOp() == "+") {
            return Builder->CreateAdd(leftVal, rightVal, "add");
        }
        else if(node->getBinaryOp() == "-") {
            return Builder->CreateSub(leftVal, rightVal, "sub");
        }
        else if(node->getBinaryOp() == ">") {
            if(node->getLeft()->getType()=="int"){
                return Builder->CreateICmpSGT(leftVal, rightVal, "SGTCmp");
            }
            else{
                return Builder->CreateICmpUGT(leftVal, rightVal, "UGTCmp");
            }
        }
        else if(node->getBinaryOp() == "<") {
            if(node->getLeft()->getType()=="int"){
                return Builder->CreateICmpSLT(leftVal, rightVal, "SLTCmp");
            }
            else{
                return Builder->CreateICmpULT(leftVal, rightVal, "ULTCmp");
            }
        }
        else if(node->getBinaryOp() == ">=") {
            if(node->getLeft()->getType()=="int"){
                return Builder->CreateICmpSGE(leftVal, rightVal, "SGTECmp");
            }
            else{
                return Builder->CreateICmpUGE(leftVal, rightVal, "UGTECmp");
            }
        }
        else if(node->getBinaryOp() == "<=") {
            if(node->getLeft()->getType()=="int"){
                return Builder->CreateICmpSLE(leftVal, rightVal, "SLTECmp");
            }
            else{
                return Builder->CreateICmpULE(leftVal, rightVal, "ULTECmp");
            }
        }
        else if(node->getBinaryOp() == "==") {
            return Builder->CreateICmpEQ(leftVal, rightVal, "EQCmp");
        }
        else if(node->getBinaryOp() == "!=") {
            return Builder->CreateICmpNE(leftVal, rightVal, "NECmp");
        }
        else if(node->getBinaryOp() == "&&") {
            return Builder->CreateAnd(leftVal, rightVal, "AndOp");
        }
        else if(node->getBinaryOp() == "||") {
            return Builder->CreateOr(leftVal, rightVal, "OrOp");
        }
    }

    antlrcpp::Any visit(TernaryOpExprAstNode *node) override {  

        Function *TheF = Builder->GetInsertBlock()->getParent();
        BasicBlock *FirstBB = BasicBlock::Create(*TheContext, "first", TheF);
        BasicBlock *SecondBB = BasicBlock::Create(*TheContext, "second");
        BasicBlock *MergeBB = BasicBlock::Create(*TheContext, "merge");
        
        Value *condVal = node->getCondition()->accept(this);
        Builder->CreateCondBr(condVal, FirstBB, SecondBB);
        
        Builder->SetInsertPoint(FirstBB);
        Value* firstVal = node->getFirst()->accept(this);
        Builder->CreateBr(MergeBB);
        FirstBB = Builder->GetInsertBlock();

        TheF->getBasicBlockList().push_back(SecondBB);
        Builder->SetInsertPoint(SecondBB);
        Value* secondVal = node->getSecond()->accept(this);
        Builder->CreateBr(MergeBB);
        SecondBB = Builder->GetInsertBlock();

        TheF->getBasicBlockList().push_back(MergeBB);
        Builder->SetInsertPoint(MergeBB);

        PHINode *PN = Builder->CreatePHI(mapToLLVMType(node->getType()), 2, "Ternary");
        PN->addIncoming(firstVal, FirstBB);
        PN->addIncoming(secondVal, SecondBB);
        return (Value*)PN;
    }

    antlrcpp::Any visit(ConstantExprAstNode *node) override {
        if(node->getIdentifier() != NULL) {
            return node->getIdentifier()->accept(this);
        }
        else{
            if(node->getConstant() == "true" || node->getConstant() == "false") {
                if(node->getConstant() == "true") {
                    return (Value*)ConstantInt::get(*TheContext, APInt(1, "1", 10));
                }
                else{
                    return (Value*)ConstantInt::get(*TheContext, APInt(1, "0", 10));
                }
            }
            if(node->getConstant()[0] == '\'' || node->getConstant() == "EOF") {
                return (Value*)ConstantInt::get(*TheContext, APInt(8, to_string((int)node->getConstant()[1]), 10));
            }
            else{
                return (Value*)ConstantInt::get(*TheContext, APInt(32, node->getConstant(), 10));
            }
        }
    }

    antlrcpp::Any visit(ReadIntExprAstNode *node) override {
        Value *scanfFormat = Builder->CreateGlobalStringPtr("%d");
        
        Value *addr = Builder->CreateAlloca(mapToLLVMType("int"), nullptr, "temp");
        vector<Value *> scanfArgs = {scanfFormat, addr};

        Function *theScanf = TheModule->getFunction("scanf");
        Builder->CreateCall(theScanf, scanfArgs);

        return (Value*)Builder->CreateLoad(addr);
    }

    antlrcpp::Any visit(ReadCharExprAstNode *node) override {
        Value *scanfFormat = Builder->CreateGlobalStringPtr("%c");
        
        Value *addr = Builder->CreateAlloca(mapToLLVMType("char"), nullptr, "temp");
        vector<Value *> scanfArgs = {scanfFormat, addr};

        Function *theScanf = TheModule->getFunction("scanf");
        Builder->CreateCall(theScanf, scanfArgs);

        return (Value*)Builder->CreateLoad(addr);
    }

    antlrcpp::Any visit(IdentifierAstNode *node) override {
        return (Value*)Builder->CreateLoad(getAddrWithOffset(node));
    }

};