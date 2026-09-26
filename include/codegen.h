#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <string>
#include <sstream>

class CodeGenerator {
private:
    std::ostringstream code;
    int indentLevel;

    void indent();
    void generateNode(const ASTNode* node);
    void generateVarDecl(const VarDeclNode* node);
    void generateAssign(const AssignNode* node);
    void generatePrint(const PrintNode* node);
    void generateIf(const IfNode* node);
    void generateWhile(const WhileNode* node);
    void generateExpr(const ASTNode* expr);

public:
    CodeGenerator() : indentLevel(0) {}
    std::string generate(const ProgramNode* program);
};

#endif