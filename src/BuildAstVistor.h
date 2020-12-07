#include <iostream>
#include "antlr4-runtime.h"
#include "Ast.h"
#include "MiniCBaseVisitor.h"

using namespace std;
using namespace miniC;

class  BuildASTVisitor : public MiniCBaseVisitor{
public:

    antlrcpp::Any visitProg(MiniCParser::ProgContext *ctx) override {
        ProgAstNode *node = new ProgAstNode();
        for(auto decl:ctx->decl()){
            node->addDecl(visit(decl));
        }
        cout << "Number of declarations in program: " << node->getDeclList().size() << endl;
        return node;
    }

    antlrcpp::Any visitDeclFunctionDecl(MiniCParser::DeclFunctionDeclContext *ctx) override {
        FunctionDeclAstNode *node = new FunctionDeclAstNode();
        node->setType(ctx->functionDecl()->TYPE()->getText());
        cout << "Function: " << ctx->functionDecl()->ID()->getText() << endl;
        node->setIdentifier(ctx->functionDecl()->ID()->getText());
        if(ctx->functionDecl()->params() != NULL ){
            node->setParamsList(visit(ctx->functionDecl()->params()));
        }
        node->setStatementBlock(visit(ctx->functionDecl()->statementBlock()));
        cout << "St&Decl Count: " << node->getStatementBlock()->getStatementsAndDecls().size() << endl;
        return (DeclAstNode*)node;
    }

    antlrcpp::Any visitParams(MiniCParser::ParamsContext *ctx) override {
        vector<VariableDeclAstNode*> paramsList;
        for(auto p:ctx->variable()){
            VariableDeclAstNode *node = new VariableDeclAstNode();
            node->setType(p->TYPE()->getText());
            node->setIdentifier(visit(p->identifier()));
            paramsList.push_back(node);
        }
        return paramsList;
    }

    antlrcpp::Any visitDeclVariableDecl(MiniCParser::DeclVariableDeclContext *ctx) override {        
        return visit(ctx->variableDecl());
    }

    antlrcpp::Any visitVariableDeclVariable(MiniCParser::VariableDeclVariableContext *ctx) override {
        VariableDeclAstNode *node = new VariableDeclAstNode();
        node->setIsInitialized(false);
        node->setType(ctx->variable()->TYPE()->getText());
        node->setIdentifier(visit(ctx->variable()->identifier()));
        return (DeclAstNode*)node;
    }

    antlrcpp::Any visitVariableDeclAssignment(MiniCParser::VariableDeclAssignmentContext *ctx) override {
        VariableDeclAstNode *node = new VariableDeclAstNode();
        node->setIsInitialized(true);
        node->setType(ctx->TYPE()->getText());
        node->setIdentifier(visit(ctx->identifier()));
        node->setAssignedValue(visit(ctx->expr()));
        return (DeclAstNode*)node;
    }

    antlrcpp::Any visitAssignStatementNormal(MiniCParser::AssignStatementNormalContext *ctx) override {
        AssignStatementAstNode *node = new AssignStatementAstNode();
        node->setIdentifier(visit(ctx->identifier()));
        node->setRhsExpr(visit(ctx->expr()));
        return (AstNode*)node;
    }

    antlrcpp::Any visitAssignStatementUnary(MiniCParser::AssignStatementUnaryContext *ctx) override {
        AssignStatementAstNode *node = new AssignStatementAstNode();
        node->setIdentifier(visit(ctx->identifier()));
        node->setUnaryOp(ctx->UNARYOP()->getText());
        return (AstNode*)node;
    }

    antlrcpp::Any visitStatementBlockStatement(MiniCParser::StatementBlockStatementContext *ctx) override {
        StatementBlockAstNode *node = new StatementBlockAstNode();
        node->addStatementOrDecl(visit(ctx->statement()));
        StatementBlockAstNode *temp = visit(ctx->statementBlock());
        for(auto stAndDec : temp->getStatementsAndDecls()) {
            node->addStatementOrDecl(stAndDec);
        }
        return node;
    }

    antlrcpp::Any visitStatementBlockVariableDecl(MiniCParser::StatementBlockVariableDeclContext *ctx) override {
        StatementBlockAstNode *node = new StatementBlockAstNode();
        node->addStatementOrDecl((AstNode*)(DeclAstNode*)visit(ctx->variableDecl()));
        StatementBlockAstNode *temp = visit(ctx->statementBlock());
        for(auto stAndDec : temp->getStatementsAndDecls()) {
            node->addStatementOrDecl(stAndDec);
        }
        return node;
    }

    antlrcpp::Any visitStatementBlockEmpty(MiniCParser::StatementBlockEmptyContext *ctx) override {
        StatementBlockAstNode *node = new StatementBlockAstNode();
        return node;
    }

    antlrcpp::Any visitStatementAssignStatement(MiniCParser::StatementAssignStatementContext *ctx) override {
        return visit(ctx->assignStatement());
    }

    antlrcpp::Any visitStatementForStatement(MiniCParser::StatementForStatementContext *ctx) override {        
        ForStatementAstNode *node = new ForStatementAstNode();
        node->setInitialization(visit(ctx->forStatement()->assignStatement()[0]));
        node->setCondition(visit(ctx->forStatement()->expr()));
        node->setStep(visit(ctx->forStatement()->assignStatement()[1]));
        node->setStatementBlock(visit(ctx->forStatement()->statementBlock()));
        return (AstNode*)node;
    }

    antlrcpp::Any visitStatementWhileStatement(MiniCParser::StatementWhileStatementContext *ctx) override {
        WhileStatementAstNode *node = new WhileStatementAstNode();
        node->setCondition(visit(ctx->whileStatement()->expr()));
        node->setStatementBlock(visit(ctx->whileStatement()->statementBlock()));
        return (AstNode*)node;
    }

    antlrcpp::Any visitStatementIfStatement(MiniCParser::StatementIfStatementContext *ctx) override {
        IfStatementAstNode *node = new IfStatementAstNode();
        node->setCondition(visit(ctx->ifStatement()->expr()));
        node->setStatementBlock(visit(ctx->ifStatement()->statementBlock()));
        return (AstNode*)node;
    }

    antlrcpp::Any visitStatementIfElseStatement(MiniCParser::StatementIfElseStatementContext *ctx) override {
        IfElseStatementAstNode *node = new IfElseStatementAstNode();
        node->setCondition(visit(ctx->ifElseStatement()->expr()));
        node->setIfStatementBlock(visit(ctx->ifElseStatement()->statementBlock()[0]));
        node->setElseStatementBlock(visit(ctx->ifElseStatement()->statementBlock()[1]));
        return (AstNode*)node;
    }

    antlrcpp::Any visitStatementBreakStatement(MiniCParser::StatementBreakStatementContext *ctx) override {
        BreakStatementAstNode *node = new BreakStatementAstNode();
        return (AstNode*)node;
    }

    antlrcpp::Any visitStatementContinueStatement(MiniCParser::StatementContinueStatementContext *ctx) override {
        ContinueStatementAstNode *node = new ContinueStatementAstNode();
        return (AstNode*)node;
    }

    antlrcpp::Any visitStatementReturnStatement(MiniCParser::StatementReturnStatementContext *ctx) override {
        ReturnStatementAstNode *node = new ReturnStatementAstNode();
        node->setExpr(visit(ctx->returnStatement()->expr()));
        return (AstNode*)node;
    }

    antlrcpp::Any visitStatementPrint(MiniCParser::StatementPrintContext *ctx) override {
        PrintStatementAstNode *node = new PrintStatementAstNode();
        if(ctx->print()->callParams() != NULL){
            node->setCallParams(visit(ctx->print()->callParams()));
        }
        return (AstNode*)node;
    }

    antlrcpp::Any visitStatementReadString(MiniCParser::StatementReadStringContext *ctx) override {
        ReadStringStatementAstNode *node = new ReadStringStatementAstNode();
        node->setCharArrayId(visit(ctx->readString()->identifier()));
        return (AstNode*)node;
    }

    antlrcpp::Any visitStatementCallStatement(MiniCParser::StatementCallStatementContext *ctx) override {
        CallExprAstNode *node = new CallExprAstNode();
        node->setIdentifier(ctx->call()->ID()->getText());
        if(ctx->call()->callParams() != NULL){
            node->setCallParams(visit(ctx->call()->callParams()));
        }
        return (AstNode*)node;
    }

    antlrcpp::Any visitCallParams(MiniCParser::CallParamsContext *ctx) override {
        vector<ExprAstNode*> callParams;
        for(auto p:ctx->expr()){
            callParams.push_back(visit(p));
        }
        return callParams;
    }

    antlrcpp::Any visitExprMulDivMod(MiniCParser::ExprMulDivModContext *ctx) override {
        BinaryOpExprAstNode* node = new BinaryOpExprAstNode();
        node->setLeft(visit(ctx->expr()[0]));
        node->setRight(visit(ctx->expr()[1]));
        node->setBinaryOp(ctx->op->getText());
        return (ExprAstNode*)node;
    }

    antlrcpp::Any visitExprNot(MiniCParser::ExprNotContext *ctx) override {
        UnaryOpExprAstNode* node = new UnaryOpExprAstNode();
        node->setExpr(visit(ctx->expr()));
        node->setUnaryOp("!");
        return (ExprAstNode*)node;
    }

    antlrcpp::Any visitExprBoolOr(MiniCParser::ExprBoolOrContext *ctx) override {
        BinaryOpExprAstNode* node = new BinaryOpExprAstNode();
        node->setLeft(visit(ctx->expr()[0]));
        node->setRight(visit(ctx->expr()[1]));
        node->setBinaryOp("||");
        return (ExprAstNode*)node;
    }

    antlrcpp::Any visitExprNegate(MiniCParser::ExprNegateContext *ctx) override {
        UnaryOpExprAstNode* node = new UnaryOpExprAstNode();
        node->setExpr(visit(ctx->expr()));
        node->setUnaryOp("-");
        return (ExprAstNode*)node;
    }

    antlrcpp::Any visitExprAddSub(MiniCParser::ExprAddSubContext *ctx) override {
        BinaryOpExprAstNode* node = new BinaryOpExprAstNode();
        node->setLeft(visit(ctx->expr()[0]));
        node->setRight(visit(ctx->expr()[1]));
        node->setBinaryOp(ctx->op->getText());
        return (ExprAstNode*)node;
    }

    antlrcpp::Any visitExprLtGt(MiniCParser::ExprLtGtContext *ctx) override {
        BinaryOpExprAstNode* node = new BinaryOpExprAstNode();
        node->setLeft(visit(ctx->expr()[0]));
        node->setRight(visit(ctx->expr()[1]));
        node->setBinaryOp(ctx->op->getText());
        return (ExprAstNode*)node;
    }

    antlrcpp::Any visitExprIdentifier(MiniCParser::ExprIdentifierContext *ctx) override {
        ConstantExprAstNode* node = new ConstantExprAstNode();
        node->setIdentifier(visit(ctx->identifier()));
        return (ExprAstNode*)node;
    }

    antlrcpp::Any visitExprBoolAnd(MiniCParser::ExprBoolAndContext *ctx) override {
        BinaryOpExprAstNode* node = new BinaryOpExprAstNode();
        node->setLeft(visit(ctx->expr()[0]));
        node->setRight(visit(ctx->expr()[1]));
        node->setBinaryOp("&&");
        return (ExprAstNode*)node;
    }

    antlrcpp::Any visitExprUnaryOP(MiniCParser::ExprUnaryOPContext *ctx) override {
        UnaryOpExprAstNode* node = new UnaryOpExprAstNode();
        node->setIdentifier(visit(ctx->identifier()));
        node->setUnaryOp(ctx->UNARYOP()->getText());
        return (ExprAstNode*)node;
    }

    antlrcpp::Any visitExprEquals(MiniCParser::ExprEqualsContext *ctx) override {
        BinaryOpExprAstNode* node = new BinaryOpExprAstNode();
        node->setLeft(visit(ctx->expr()[0]));
        node->setRight(visit(ctx->expr()[1]));
        node->setBinaryOp(ctx->op->getText());
        return (ExprAstNode*)node;
    }

    antlrcpp::Any visitExprCast(MiniCParser::ExprCastContext *ctx) override {
        UnaryOpExprAstNode* node = new UnaryOpExprAstNode();
        node->setExpr(visit(ctx->expr()));
        node->setUnaryOp(ctx->TYPE()->getText());
        return (ExprAstNode*)node;
    }

    antlrcpp::Any visitExprCall(MiniCParser::ExprCallContext *ctx) override {
        CallExprAstNode *node = new CallExprAstNode();
        node->setIdentifier(ctx->call()->ID()->getText());
        if(ctx->call()->callParams() != NULL){
            node->setCallParams(visit(ctx->call()->callParams()));
        }
        return (ExprAstNode*)node;
    }

    antlrcpp::Any visitExprTernary(MiniCParser::ExprTernaryContext *ctx) override {
        TernaryOpExprAstNode* node = new TernaryOpExprAstNode();
        node->setCondition(visit(ctx->expr()[0]));
        node->setFirst(visit(ctx->expr()[1]));
        node->setSecond(visit(ctx->expr()[2]));
        return (ExprAstNode*)node;
    }

    antlrcpp::Any visitExprConstant(MiniCParser::ExprConstantContext *ctx) override {
        ConstantExprAstNode* node = new ConstantExprAstNode();
        node->setConstant(ctx->CONSTANT()->getText());
        return (ExprAstNode*)node;
    }

    antlrcpp::Any visitExprReadInt(MiniCParser::ExprReadIntContext *ctx) override {
        ReadIntExprAstNode* node = new ReadIntExprAstNode();
        return (ExprAstNode*)node;
    }

    antlrcpp::Any visitExprReadChar(MiniCParser::ExprReadCharContext *ctx) override {
        ReadCharExprAstNode* node = new ReadCharExprAstNode();
        return (ExprAstNode*)node;
    }

    antlrcpp::Any visitExprParentheses(MiniCParser::ExprParenthesesContext *ctx) override {
        return visit(ctx->expr());
    }

    antlrcpp::Any visitIdentifier(MiniCParser::IdentifierContext *ctx) override {
        IdentifierAstNode* node = new IdentifierAstNode();
        node->setID(ctx->ID()->getText());
        if(ctx->expr().size() >= 1) {
            node->setRowExpr(visit(ctx->expr()[0]));
        }
        if(ctx->expr().size() == 2) {
            node->setColExpr(visit(ctx->expr()[1]));
        }
        return node;
    }

};