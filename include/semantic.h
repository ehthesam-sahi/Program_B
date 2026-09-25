#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "symboltable.h"
#include <string>
#include <vector>

class SemanticAnalyzer {
private:
    SymbolTable symbolTable;
    std::vector<std::string> errors;

    void analyzeNode(const ASTNode* node);
    void analyzeVarDecl(const VarDeclNode* node);
    void analyzeAssign(const AssignNode* node);
    void analyzePrint(const PrintNode* node);
    void analyzeIf(const IfNode* node);
    void analyzeWhile(const WhileNode* node);

    std::string evaluateType(const ASTNode* expr);

public:
    SemanticAnalyzer() = default;
    bool analyze(const ProgramNode* program);
    void printErrors() const;
    const SymbolTable& getSymbolTable() const { return symbolTable; }
};

#endif