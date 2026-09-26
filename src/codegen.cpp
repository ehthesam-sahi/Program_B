#include "../include/codegen.h"on 
#include <iostream>

void CodeGenerator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        code << "    ";
    }
}

std::string CodeGenerator::generate(const ProgramNode* program) {
    code.str("");
    code.clear();
    indentLevel = 0;

    code << "# Generated Target Code (Python)\n";
    code << "# Transpiled from Invented DSL\n\n";

    if (!program) return code.str();

    for (const auto& stmt : program->statements) {
        if (stmt) {
            generateNode(stmt.get());
        }
    }

    return code.str();
}

void CodeGenerator::generateNode(const ASTNode* node) {
    if (!node) return;

    if (auto varDecl = dynamic_cast<const VarDeclNode*>(node)) {
        generateVarDecl(varDecl);
    } else if (auto assign = dynamic_cast<const AssignNode*>(node)) {
        generateAssign(assign);
    } else if (auto print = dynamic_cast<const PrintNode*>(node)) {
        generatePrint(print);
    } else if (auto ifNode = dynamic_cast<const IfNode*>(node)) {
        generateIf(ifNode);
    } else if (auto whileNode = dynamic_cast<const WhileNode*>(node)) {
        generateWhile(whileNode);
    }
}

void CodeGenerator::generateVarDecl(const VarDeclNode* node) {
    indent();
    code << node->name << " = ";
    if (node->value) {
        generateExpr(node->value.get());
    } else {
        code << (node->type == "text" ? "\"\"" : "0");
    }
    code << "\n";
}

void CodeGenerator::generateAssign(const AssignNode* node) {
    indent();
    code << node->name << " = ";
    if (node->value) {
        generateExpr(node->value.get());
    }
    code << "\n";
}

void CodeGenerator::generatePrint(const PrintNode* node) {
    indent();
    code << "print(";
    if (node->expression) {
        generateExpr(node->expression.get());
    }
    code << ")\n";
}

void CodeGenerator::generateIf(const IfNode* node) {
    indent();
    code << "if ";
    if (node->condition) {
        generateExpr(node->condition.get());
    }
    code << ":\n";

    indentLevel++;
    if (node->thenBranch.empty()) {
        indent();
        code << "pass\n";
    } else {
        for (const auto& stmt : node->thenBranch) {
            if (stmt) generateNode(stmt.get());
        }
    }
    indentLevel--;

    if (!node->elseBranch.empty()) {
        indent();
        code << "else:\n";
        indentLevel++;
        for (const auto& stmt : node->elseBranch) {
            if (stmt) generateNode(stmt.get());
        }
        indentLevel--;
    }
}

void CodeGenerator::generateWhile(const WhileNode* node) {
    indent();
    code << "while ";
    if (node->condition) {
        generateExpr(node->condition.get());
    }
    code << ":\n";

    indentLevel++;
    if (node->body.empty()) {
        indent();
        code << "pass\n";
    } else {
        for (const auto& stmt : node->body) {
            if (stmt) generateNode(stmt.get());
        }
    }
    indentLevel--;
}

void CodeGenerator::generateExpr(const ASTNode* expr) {
    if (!expr) return;

    if (auto num = dynamic_cast<const NumberNode*>(expr)) {
        std::string str = std::to_string(num->value);
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        if (str.back() == '.') str.pop_back();
        code << str;
    } else if (auto str = dynamic_cast<const StringNode*>(expr)) {
        code << "\"" << str->value << "\"";
    } else if (auto var = dynamic_cast<const VariableNode*>(expr)) {
        code << var->name;
    } else if (auto bin = dynamic_cast<const BinaryOpNode*>(expr)) {
        code << "(";
        generateExpr(bin->left.get());
        code << " " << bin->op << " ";
        generateExpr(bin->right.get());
        code << ")";
    }
}