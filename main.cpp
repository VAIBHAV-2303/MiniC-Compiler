#include <iostream>
#include "antlr4-runtime.h"
#include "MiniCLexer.cpp"
#include "MiniCParser.cpp"
#include "BuildAstVistor.h"
#include "SemanticAnalysisVisitor.h"
#include "IRGenVisitor.h"

using namespace std;
using namespace miniC;
using namespace antlr4;

int main(int argc, const char* argv[]) {

    ifstream stream;
    stream.open(argv[1]);

    ANTLRInputStream input(stream);
    MiniCLexer lexer(&input);
    CommonTokenStream tokens(&lexer);

    // tokens.fill();
    // for (auto token : tokens.getTokens()) {
    //     cout << token->toString() << std::endl;
    // }

    MiniCParser parser(&tokens);    

    MiniCParser::ProgContext* tree = parser.prog();
    
    // Building AST using Parse tree visitors
    cout << "Building AST" << endl;
    BuildASTVisitor visitor;
    ProgAstNode *AstRoot = visitor.visitProg(tree);
    cout << "\n===================================\n\n";

    // Semantic Analysis Phase
    cout << "Semantic Analysis Phase" << endl;
    SemanticAnalysisVisitor *semanticvisitor = new SemanticAnalysisVisitor();
    SymbolTable *table = (SymbolTable*)AstRoot->accept(semanticvisitor);
    cout << "\n===================================\n\n";

    // IR Generation Phase
    cout << "IR Generation Phase\n\n";
    IRGenVisitor *IRVisitor = new IRGenVisitor();
    string IR = AstRoot->accept(IRVisitor);

    // Storing the IR to a .ll file
    ofstream file("temp.ll");
    file << IR;

    return 0;
}