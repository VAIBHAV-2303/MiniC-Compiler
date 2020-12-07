#include "antlr4-runtime.h"
#include<bits/stdc++.h>

using namespace std;

class ProgAstNode;
class DeclAstNode;
class FunctionDeclAstNode;
class VariableDeclAstNode;
class AssignStatementAstNode;
class StatementBlockAstNode;
class ForStatementAstNode;
class WhileStatementAstNode;
class IfStatementAstNode;
class IfElseStatementAstNode;
class BreakStatementAstNode;
class ContinueStatementAstNode;
class ReturnStatementAstNode;
class PrintStatementAstNode;
class ReadStringStatementAstNode;
class ExprAstNode;
class CallExprAstNode;
class ReadIntExprAstNode;
class ReadCharExprAstNode;
class UnaryOpExprAstNode;
class BinaryOpExprAstNode;
class TernaryOpExprAstNode;
class ConstantExprAstNode;
class IdentifierAstNode;

class ASTvisitor {
    public:
    
    virtual antlrcpp::Any visit(ProgAstNode *node){}
    virtual antlrcpp::Any visit(FunctionDeclAstNode *node){}
    virtual antlrcpp::Any visit(VariableDeclAstNode *node){}
    virtual antlrcpp::Any visit(AssignStatementAstNode *node){}
    virtual antlrcpp::Any visit(StatementBlockAstNode *node){}
    virtual antlrcpp::Any visit(ForStatementAstNode *node){}
    virtual antlrcpp::Any visit(WhileStatementAstNode *node){}
    virtual antlrcpp::Any visit(IfStatementAstNode *node){}
    virtual antlrcpp::Any visit(IfElseStatementAstNode *node){}
    virtual antlrcpp::Any visit(BreakStatementAstNode *node){}
    virtual antlrcpp::Any visit(ContinueStatementAstNode *node){}
    virtual antlrcpp::Any visit(ReturnStatementAstNode *node){}
    virtual antlrcpp::Any visit(CallExprAstNode *node){}
    virtual antlrcpp::Any visit(UnaryOpExprAstNode *node){}
    virtual antlrcpp::Any visit(BinaryOpExprAstNode *node){}
    virtual antlrcpp::Any visit(TernaryOpExprAstNode *node){}
    virtual antlrcpp::Any visit(ConstantExprAstNode *node){}
    virtual antlrcpp::Any visit(IdentifierAstNode *node){}
    virtual antlrcpp::Any visit(PrintStatementAstNode *node){}
    virtual antlrcpp::Any visit(ReadStringStatementAstNode *node){}
    virtual antlrcpp::Any visit(ReadIntExprAstNode *node){}
    virtual antlrcpp::Any visit(ReadCharExprAstNode *node){}
};

class AstNode{
    public:

    virtual antlrcpp::Any accept(ASTvisitor *visitor){}
};

class ProgAstNode : public AstNode {
     
    vector<DeclAstNode*>declList;

    public:

    vector<DeclAstNode*> getDeclList() {return declList;}

    void addDecl(DeclAstNode* decl) {declList.push_back(decl);}
    
    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class DeclAstNode : public AstNode {
    string type;

    public:

    string getType() {return type;}

    void setType(string _type) {type = _type;}
};

class FunctionDeclAstNode : public DeclAstNode {
    string identifier;
    vector<VariableDeclAstNode*> paramsList;
    StatementBlockAstNode* statementBlock;

    public:

    string getIdentifier() {return identifier;}
    vector<VariableDeclAstNode*> getParamsList() {return paramsList;}
    StatementBlockAstNode* getStatementBlock() {return statementBlock;}

    void setIdentifier(string _identifier) {identifier = _identifier;}
    void setStatementBlock(StatementBlockAstNode* _statementBlock) {statementBlock = _statementBlock;}
    void setParamsList(vector<VariableDeclAstNode*> _paramsList) {paramsList = _paramsList;}

    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class VariableDeclAstNode : public DeclAstNode {
    
    IdentifierAstNode* identifier;
    bool isInitialized;
    ExprAstNode* assignedValue;

    public:

    IdentifierAstNode* getIdentifier() {return identifier;}
    bool getIsInitialized() {return isInitialized;}
    ExprAstNode* getAssignedValue() {return assignedValue;}

    void setIdentifier(IdentifierAstNode* _identifier) {identifier = _identifier;}
    void setIsInitialized(bool _isInitialized) {isInitialized = _isInitialized;}
    void setAssignedValue(ExprAstNode* _assignedValue) {assignedValue = _assignedValue;}

    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class AssignStatementAstNode : public AstNode {
    
    IdentifierAstNode* identifier;
    ExprAstNode* rhsExpr;
    string unaryOp;

    public:

    IdentifierAstNode* getIdentifier() {return identifier;}
    ExprAstNode* getRhsExpr() {return rhsExpr;}
    string getUnaryOp() {return unaryOp;}

    void setIdentifier(IdentifierAstNode* _identifier) {identifier = _identifier;}
    void setRhsExpr(ExprAstNode* _rhsExpr) {rhsExpr = _rhsExpr;}
    void setUnaryOp(string _unaryOp) {unaryOp = _unaryOp;}

    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class StatementBlockAstNode : public AstNode {

    vector<AstNode*> statementsAndDecls;

    public:

    vector<AstNode*> getStatementsAndDecls() {return statementsAndDecls;}

    void addStatementOrDecl(AstNode* statementOrDecl) {
        statementsAndDecls.push_back(statementOrDecl);
    }

    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class ForStatementAstNode : public AstNode {
    
    AstNode* initialization;
    ExprAstNode* condition;
    AstNode* step;
    StatementBlockAstNode* statementBlock;

    public:

    AstNode* getInitialization() {return initialization;}
    ExprAstNode* getCondition() {return condition;}
    AstNode* getStep() {return step;}
    StatementBlockAstNode* getStatementBlock() {return statementBlock;}

    void setInitialization(AstNode* _initialization) {initialization = _initialization;}
    void setCondition(ExprAstNode* _condition) {condition = _condition;}
    void setStep(AstNode* _step) {step = _step;}
    void setStatementBlock(StatementBlockAstNode* _statementBlock) {statementBlock = _statementBlock;}

    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class WhileStatementAstNode : public AstNode {
    
    ExprAstNode* condition;
    StatementBlockAstNode* statementBlock;

    public:
    
    ExprAstNode* getCondition() {return condition;}
    StatementBlockAstNode* getStatementBlock() {return statementBlock;}

    void setCondition(ExprAstNode* _condition) {condition = _condition;}
    void setStatementBlock(StatementBlockAstNode* _statementBlock) {statementBlock = _statementBlock;}

    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class IfStatementAstNode : public AstNode {
    
    ExprAstNode* condition;
    StatementBlockAstNode* statementBlock;

    public:
    
    ExprAstNode* getCondition() {return condition;}
    StatementBlockAstNode* getStatementBlock() {return statementBlock;}

    void setCondition(ExprAstNode* _condition) {condition = _condition;}
    void setStatementBlock(StatementBlockAstNode* _statementBlock) {statementBlock = _statementBlock;}

    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class IfElseStatementAstNode : public AstNode {
    
    ExprAstNode* condition;
    StatementBlockAstNode* ifStatementBlock;
    StatementBlockAstNode* elseStatementBlock;

    public:
    
    ExprAstNode* getCondition() {return condition;}
    StatementBlockAstNode* getIfStatementBlock() {return ifStatementBlock;}
    StatementBlockAstNode* getElseStatementBlock() {return elseStatementBlock;}

    void setCondition(ExprAstNode* _condition) {condition = _condition;}
    void setIfStatementBlock(StatementBlockAstNode* _ifStatementBlock) {ifStatementBlock = _ifStatementBlock;}
    void setElseStatementBlock(StatementBlockAstNode* _elseStatementBlock) {elseStatementBlock = _elseStatementBlock;}

    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class BreakStatementAstNode : public AstNode {    
    public:
    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class ContinueStatementAstNode : public AstNode {  
    public:
    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class ReturnStatementAstNode : public AstNode {
    
    ExprAstNode* expr;

    public:

    ExprAstNode* getExpr() {return expr;}

    void setExpr(ExprAstNode* _expr) {expr = _expr;}

    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class PrintStatementAstNode : public AstNode {
    
    vector<ExprAstNode*> callParams;

    public:

    vector<ExprAstNode*> getCallParams() {return callParams;}

    void setCallParams(vector<ExprAstNode*> _callParams) {callParams = _callParams;}

    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class ReadStringStatementAstNode : public AstNode {
    
    IdentifierAstNode* charArrayId;

    public:

    IdentifierAstNode* getCharArrayId() {return charArrayId;}

    void setCharArrayId(IdentifierAstNode* _charArrayId) {charArrayId = _charArrayId;}

    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class ExprAstNode : public AstNode {

    string type;

    public:

    string getType() {return type;}

    void setType(string _type) {type = _type;}

};

class CallExprAstNode : public ExprAstNode {

    string identifier;
    vector<ExprAstNode*> callParams;

    public:

    string getIdentifier() {return identifier;}
    vector<ExprAstNode*> getCallParams() {return callParams;}

    void setIdentifier(string _identifier) {identifier = _identifier;}
    void setCallParams(vector<ExprAstNode*> _callParams) {callParams = _callParams;}

    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class UnaryOpExprAstNode : public ExprAstNode {
    
    string unaryOp;
    ExprAstNode* expr;
    IdentifierAstNode* identifier;
  
    public:

    string getUnaryOp() {return unaryOp;}
    ExprAstNode* getExpr() {return expr;}
    IdentifierAstNode* getIdentifier() {return identifier;}

    void setUnaryOp(string _unaryOp) {unaryOp = _unaryOp;}
    void setExpr(ExprAstNode* _expr) {expr = _expr;}
    void setIdentifier(IdentifierAstNode* _identifier) {identifier = _identifier;}

    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class BinaryOpExprAstNode : public ExprAstNode {
    string binaryOp;
    ExprAstNode* left;
    ExprAstNode* right;

    public:

    string getBinaryOp() {return binaryOp;}
    ExprAstNode* getLeft() {return left;}
    ExprAstNode* getRight() {return right;}

    void setBinaryOp(string _binaryOp) {binaryOp = _binaryOp;}
    void setLeft(ExprAstNode* _left) {left = _left;}
    void setRight(ExprAstNode* _right) {right = _right;}

    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class TernaryOpExprAstNode : public ExprAstNode {
    
    ExprAstNode* condition;
    ExprAstNode* first;
    ExprAstNode* second;

    public:

    ExprAstNode* getCondition() {return condition;}
    ExprAstNode* getFirst() {return first;}
    ExprAstNode* getSecond() {return second;}

    void setCondition(ExprAstNode* _condition) {condition = _condition;}
    void setFirst(ExprAstNode* _first) {first = _first;}
    void setSecond(ExprAstNode* _second) {second = _second;}

    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class ConstantExprAstNode : public ExprAstNode {

    IdentifierAstNode* identifier;
    string constant;

    public:

    IdentifierAstNode* getIdentifier() {return identifier;}
    string getConstant() {return constant;}
    
    void setIdentifier(IdentifierAstNode* _identifier) {identifier = _identifier;}
    void setConstant(string _constant) {constant = _constant;}
    
    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class ReadIntExprAstNode : public ExprAstNode {

    public:

    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class ReadCharExprAstNode : public ExprAstNode {

    public:

    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};

class IdentifierAstNode : public AstNode {

    string ID;
    ExprAstNode* rowExpr;
    ExprAstNode* colExpr;
    string type;

    public:

    string getID() {return ID;}
    ExprAstNode* getRowExpr() {return rowExpr;}
    ExprAstNode* getColExpr() {return colExpr;}
    string getType() {return type;}

    void setID(string _ID) {ID = _ID;}
    void setRowExpr(ExprAstNode* _rowExpr) {rowExpr = _rowExpr;}
    void setColExpr(ExprAstNode* _colExpr) {colExpr = _colExpr;}
    void setType(string _type) {type = _type;}

    antlrcpp::Any accept(ASTvisitor *visitor){
        return visitor->visit(this);
    }
};