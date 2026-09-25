#include "../include/semantic.h"
#include <iostream>

bool SemanticAnalyzer::analyze(const ProgramNode* program) {
    errors.clear();
    if (!program) return true;

    for (const auto& stmt : program->statements) {
        if (stmt) {
            analyzeNode(stmt.get());
        }
    }

    return errors.empty();
}

void SemanticAnalyzer::analyzeNode(const ASTNode* node) {
    if (!node) return;

    if (auto varDecl = dynamic_cast<const VarDeclNode*>(node)) {
        analyzeVarDecl(varDecl);
    } else if (auto assign = dynamic_cast<const AssignNode*>(node)) {
        analyzeAssign(assign);
    } else if (auto print = dynamic_cast<const PrintNode*>(node)) {
        analyzePrint(print);
    } else if (auto ifNode = dynamic_cast<const IfNode*>(node)) {
        analyzeIf(ifNode);
    } else if (auto whileNode = dynamic_cast<const WhileNode*>(node)) {
        analyzeWhile(whileNode);
    }
}

void SemanticAnalyzer::analyzeVarDecl(const VarDeclNode* node) {
    if (node->type != "number" && node->type != "text") {
        errors.push_back("Semantic Error: Invalid type '" + node->type + "' for variable '" + node->name + "'. Must be 'number' or 'text'.");
    }

    if (!symbolTable.declareVariable(node->name, node->type, 0)) {
        errors.push_back("Semantic Error: Redeclaration of variable '" + node->name + "' in current scope.");
    }

    if (node->value) {
        std::string exprType = evaluateType(node->value.get());
        if (exprType != "unknown" && exprType != node->type) {
            errors.push_back("Type Error: Cannot assign expression of type '" + exprType + "' to variable '" + node->name + "' of type '" + node->type + "'.");
        }
    }
}

void SemanticAnalyzer::analyzeAssign(const AssignNode* node) {
    if (!symbolTable.isDeclared(node->name)) {
        errors.push_back("Semantic Error: Variable '" + node->name + "' used before declaration.");
        return;
    }

    const Symbol* sym = symbolTable.lookup(node->name);
    if (node->value && sym) {
        std::string exprType = evaluateType(node->value.get());
        if (exprType != "unknown" && exprType != sym->type) {
            errors.push_back("Type Error: Cannot assign type '" + exprType + "' to variable '" + node->name + "' of type '" + sym->type + "'.");
        }
    }
}

void SemanticAnalyzer::analyzePrint(const PrintNode* node) {
    if (node->expression) {
        evaluateType(node->expression.get());
    }
}

void SemanticAnalyzer::analyzeIf(const IfNode* node) {
    if (node->condition) {
        evaluateType(node->condition.get());
    }

    symbolTable.enterScope();
    for (const auto& stmt : node->thenBranch) {
        if (stmt) analyzeNode(stmt.get());
    }
    symbolTable.exitScope();

    if (!node->elseBranch.empty()) {
        symbolTable.enterScope();
        for (const auto& stmt : node->elseBranch) {
            if (stmt) analyzeNode(stmt.get());
        }
        symbolTable.exitScope();
    }
}

void SemanticAnalyzer::analyzeWhile(const WhileNode* node) {
    if (node->condition) {
        evaluateType(node->condition.get());
    }

    symbolTable.enterScope();
    for (const auto& stmt : node->body) {
        if (stmt) analyzeNode(stmt.get());
    }
    symbolTable.exitScope();
}

std::string SemanticAnalyzer::evaluateType(const ASTNode* expr) {
    if (!expr) return "unknown";

    if (dynamic_cast<const NumberNode*>(expr)) {
        return "number";
    }
    if (dynamic_cast<const StringNode*>(expr)) {
        return "text";
    }
    if (auto varNode = dynamic_cast<const VariableNode*>(expr)) {
        if (!symbolTable.isDeclared(varNode->name)) {
            errors.push_back("Semantic Error: Variable '" + varNode->name + "' is not declared.");
            return "unknown";
        }
        return symbolTable.lookup(varNode->name)->type;
    }
    if (auto binOp = dynamic_cast<const BinaryOpNode*>(expr)) {
        std::string leftType = evaluateType(binOp->left.get());
        std::string rightType = evaluateType(binOp->right.get());

        // Comparison operations return 'number' (boolean representation)
        if (binOp->op == ">" || binOp->op == "<" || binOp->op == "==" || 
            binOp->op == "!=" || binOp->op == ">=" || binOp->op == "<=") {
            return "number";
        }

        if (leftType != "unknown" && rightType != "unknown" && leftType != rightType) {
            errors.push_back("Type Mismatch Error: Operator '" + binOp->op + "' cannot be applied to types '" + leftType + "' and '" + rightType + "'.");
            return "unknown";
        }
        return leftType;
    }

    return "unknown";
}

void SemanticAnalyzer::printErrors() const {
    if (errors.empty()) {
        std::cout << "[Semantic Pass]: No semantic or type errors found!\n";
    } else {
        std::cout << "\n=== Semantic & Type Checking Errors ===\n";
        for (const auto& err : errors) {
            std::cout << "  - " << err << "\n";
        }
        std::cout << "=======================================\n";
    }
}