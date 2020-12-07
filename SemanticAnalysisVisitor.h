#include "SymbolTable.h"

class SemanticAnalysisVisitor : public ASTvisitor {

    SymbolTable *SymTab = new SymbolTable();

    antlrcpp::Any visit(ProgAstNode *node) override {
        for(auto declaration : node->getDeclList()) {
            declaration->accept(this);
        }
        return SymTab;
    }

    antlrcpp::Any visit(FunctionDeclAstNode *node) override {
        cout << "Analyzing function: " << node->getIdentifier() << endl;
        SymTab->createChild(node->getIdentifier());

        vector<string> parTypes;
        for(auto par : node->getParamsList()) {
            cout << "Encountered parameter: " << par->getIdentifier()->getID() << endl;
            parTypes.push_back(par->getType());
            SymTab->addEntry(par->getType(), par->getIdentifier()->getID());
        }
        SymTab->addFunction(node->getIdentifier(), node->getType(), parTypes);
        node->getStatementBlock()->accept(this);

        SymTab->moveToPar();
        return 0;
    }

    antlrcpp::Any visit(VariableDeclAstNode *node) override {
        SymTab->addEntry(node->getType(), node->getIdentifier()->getID());
        node->getIdentifier()->accept(this);
        if(node->getIsInitialized()) {
            string rhsType = node->getAssignedValue()->accept(this);
            if(node->getType() != rhsType) {
                cout<<"ERROR: Conflicting types in declaration of "<<node->getIdentifier()->getID()<<endl; 
            }
        }
        return 0;
    }

    antlrcpp::Any visit(AssignStatementAstNode *node) override {
        string lhsType = node->getIdentifier()->accept(this);
        if(node->getRhsExpr() != NULL) {
            string rhsType = node->getRhsExpr()->accept(this);
            if(lhsType != rhsType) {
                cout<<"ERROR: Conflicting types in assignment of "<<node->getIdentifier()->getID()<<endl; 
            }
        }
        else{
            if(lhsType != "int" && lhsType != "uint") {
                cout<<"ERROR: Cannot apply ++\\-- op on "<<node->getIdentifier()->getID()<<endl; 
            }
        }
        return 0;
    }

    antlrcpp::Any visit(StatementBlockAstNode *node) override {
        for(auto stAndDec : node->getStatementsAndDecls()) {
            stAndDec->accept(this);
        }
        return 0;
    }

    antlrcpp::Any visit(ForStatementAstNode *node) override {
        node->getInitialization()->accept(this);
        string condType = node->getCondition()->accept(this);
        if(condType != "bool") {
            cout<<"ERROR: For loop condition should be of type bool.\n"; 
        }
        SymTab->createChild(SymTab->getCurrFunc());
        
        node->getStatementBlock()->accept(this);
        node->getStep()->accept(this);
        
        SymTab->moveToPar();
        return 0;
    }

    antlrcpp::Any visit(WhileStatementAstNode *node) override {
        string condType = node->getCondition()->accept(this);
        if(condType != "bool") {
            cout<<"ERROR: While loop condition should be of type bool.\n"; 
        }
        SymTab->createChild(SymTab->getCurrFunc());
        
        node->getStatementBlock()->accept(this);
        
        SymTab->moveToPar();
        return 0;
    }

    antlrcpp::Any visit(IfStatementAstNode *node) override {
        string condType = node->getCondition()->accept(this);
        if(condType != "bool") {
            cout<<"ERROR: If condition should be of type bool.\n"; 
        }
        SymTab->createChild(SymTab->getCurrFunc());
        
        node->getStatementBlock()->accept(this);
        
        SymTab->moveToPar();
        return 0;
    }

    antlrcpp::Any visit(IfElseStatementAstNode *node) override {
        string condType = node->getCondition()->accept(this);
        if(condType != "bool") {
            cout<<"ERROR: If-Else condition should be of type bool.\n"; 
        }
        SymTab->createChild(SymTab->getCurrFunc());

        node->getIfStatementBlock()->accept(this);
        
        SymTab->moveToPar();
        SymTab->createChild(SymTab->getCurrFunc());

        node->getElseStatementBlock()->accept(this);
        
        SymTab->moveToPar();
        return 0;
    }

    antlrcpp::Any visit(BreakStatementAstNode *node) override {
        return 0;
    }

    antlrcpp::Any visit(ContinueStatementAstNode *node) override {
        return 0;
    }

    antlrcpp::Any visit(ReturnStatementAstNode *node) override {
        string retType = node->getExpr()->accept(this);
        if(retType != SymTab->getFuncType(SymTab->getCurrFunc())) {
            cout << "ERROR: Incorrect return type in function " << SymTab->getCurrFunc() << endl;
        }
        return 0;
    }

    antlrcpp::Any visit(PrintStatementAstNode *node) override {
        for(auto callPar : node->getCallParams()) {
            callPar->accept(this);
        }
        return 0;
    }

    antlrcpp::Any visit(ReadStringStatementAstNode *node) override {
        string parType = node->getCharArrayId()->accept(this);
        if(parType != "char") {
            cout << "ERROR: ReadString function expects a charater array as parameter" << endl;
        }
        return 0;
    }

    antlrcpp::Any visit(CallExprAstNode *node) override {
        vector<string> parTypes = SymTab->getFuncParTypes(node->getIdentifier());
        int i = 0;
        for(auto callPar : node->getCallParams()) {
            string parType = callPar->accept(this);
            if(i >= parTypes.size() || parType != parTypes[i]) {
                cout << "ERROR: Incorrect Parameter list in call to " << node->getIdentifier() << endl;
    
                node->setType(SymTab->getFuncType(node->getIdentifier()));
                return SymTab->getFuncType(node->getIdentifier());
            }
            i++;
        }
        if(i != parTypes.size()) {
            cout << "ERROR: Incorrect Parameter list in call to " << node->getIdentifier() << endl;
        }
    
        node->setType(SymTab->getFuncType(node->getIdentifier()));
        return SymTab->getFuncType(node->getIdentifier());
    }

    antlrcpp::Any visit(UnaryOpExprAstNode *node) override {
        if(node->getExpr()!=NULL) {
            string exprType = node->getExpr()->accept(this);
            if(node->getUnaryOp() == "-") {
                if(exprType != "int") {
                    cout<<"ERROR: Cannot apply '-' op on a non-int expression\n"; 
                }
                
                node->setType(string("int"));
                return string("int");
            }
            else if(node->getUnaryOp() == "!") {
                if(exprType != "bool") {
                    cout<<"ERROR: Cannot apply '!' op on a non-bool expression\n"; 
                }

                node->setType(string("bool"));
                return string("bool");
            }
            else { // Type Casting
                node->setType(node->getUnaryOp());
                return node->getUnaryOp();
            }
        }
        else{
            string idType = node->getIdentifier()->accept(this);
            if(idType != "int" && idType != "uint") {
                cout<<"ERROR: Cannot apply ++\\-- op on "<<node->getIdentifier()->getID()<<endl; 
            }

            node->setType(idType);
            return idType;
        }
    }

    antlrcpp::Any visit(BinaryOpExprAstNode *node) override {
        string leftType = node->getLeft()->accept(this);
        string rightType = node->getRight()->accept(this);
        
        if(node->getBinaryOp() == "*"
        || node->getBinaryOp() == "/"
        || node->getBinaryOp() == "%"
        || node->getBinaryOp() == "+"
        || node->getBinaryOp() == "-") {
            if(leftType != rightType || (leftType != "int" && leftType != "uint") ) {
                cout << "ERROR: Cannot apply arithmetic operator on non-int expression\n";
            }

            node->setType(leftType);
            return leftType;
        }
        
        if(node->getBinaryOp() == ">"
        || node->getBinaryOp() == "<"
        || node->getBinaryOp() == ">="
        || node->getBinaryOp() == "<=") {
            if(leftType != rightType || (leftType != "int" && leftType != "uint") ) {
                cout << "ERROR: Cannot apply comparison operator on non-int expression\n";
            }

            node->setType(string("bool"));
            return string("bool");
        }

        if(node->getBinaryOp() == "==" || node->getBinaryOp() == "!=") {
            if(leftType != rightType) {
                cout << "ERROR: Cannot compare different types\n";
            }

            node->setType(string("bool"));
            return string("bool");
        }

        if(node->getBinaryOp() == "&&" || node->getBinaryOp() == "||") {
            if(leftType != rightType || leftType != "bool" ) {
                cout << "ERROR: Cannot apply boolean operator on non-bool expression\n";
            }

            node->setType(string("bool"));
            return string("bool");
        }
    }

    antlrcpp::Any visit(TernaryOpExprAstNode *node) override {
        string condType = node->getCondition()->accept(this);
        if(condType != "bool") {
            cout<<"ERROR: If condition should be of type bool.\n"; 
        }
        string firstType = node->getFirst()->accept(this);
        string secondType = node->getSecond()->accept(this);
        
        node->setType(firstType);
        return firstType;
    }

    antlrcpp::Any visit(ConstantExprAstNode *node) override {
        if(node->getIdentifier() != NULL) {

            node->setType(node->getIdentifier()->accept(this));
            return node->getType();
        }
        else{
            if(node->getConstant() == "true" || node->getConstant() == "false") {
                node->setType(string("bool"));
                return string("bool");
            }
            if(node->getConstant()[0] == '\'' || node->getConstant() == "EOF") {
                if(node->getConstant() == "'\\n'"){
                    node->setType(string("newLine"));
                }
                else{
                    node->setType(string("char"));
                }
                return string("char");
            }
            else{
                node->setType(string("int"));
                return string("int");
            }
        }
    }

    antlrcpp::Any visit(ReadIntExprAstNode *node) override {
        node->setType(string("int"));
        return string("int");
    }

    antlrcpp::Any visit(ReadCharExprAstNode *node) override {
        node->setType(string("char"));
        return string("char");
    }

    antlrcpp::Any visit(IdentifierAstNode *node) override {
        if(SymTab->foundReference(node->getID()) == false) {
            cout << "ERROR: Use of an undeclared variable " << node->getID() << endl;
        }
        else{
            if(node->getRowExpr() != NULL) {
                string rowExprType = node->getRowExpr()->accept(this);
                if(rowExprType != "int" && rowExprType != "uint") {
                    cout << "ERROR: Use of non-int expression for row indexing of " << node->getID() << endl;
                }
            }
            if(node->getColExpr() != NULL) {
                string colExprType = node->getColExpr()->accept(this);
                if(colExprType != "int" && colExprType != "uint") {
                    cout << "ERROR: Use of non-int expression for col indexing of " << node->getID() << endl;
                }
            }
            
            node->setType(SymTab->getType(node->getID()));
            return SymTab->getType(node->getID());
        }
    }

};