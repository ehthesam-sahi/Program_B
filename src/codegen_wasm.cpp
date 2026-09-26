#include "../include/codegen_wasm.h"

void WasmCodeGenerator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        wat << "  ";
    }
}

std::string WasmCodeGenerator::generate(const ProgramNode* program) {
    wat.str("");
    wat.clear();
    indentLevel = 0;

    wat << ";; WebAssembly Text Format (.wat)\n";
    wat << ";; Transpiled from Invented DSL\n";
    wat << "(module\n";
    indentLevel++;

    indent(); wat << "(import \"env\" \"print_num\" (func $print_num (param f64)))\n";
    indent(); wat << "(func (export \"main\")\n";
    indentLevel++;

    if (program) {
        // Declare local numeric variables
        for (const auto& stmt : program->statements) {
            if (auto decl = dynamic_cast<const VarDeclNode*>(stmt.get())) {
                indent(); wat << "(local $" << decl->name << " f64)\n";
            }
        }

        // Generate statements
        for (const auto& stmt : program->statements) {
            if (stmt) generateNode(stmt.get());
        }
    }

    indentLevel--;
    indent(); wat << ")\n";
    indentLevel--;
    wat << ")\n";

    return wat.str();
}

void WasmCodeGenerator::generateNode(const ASTNode* node) {
    if (!node) return;

    if (auto varDecl = dynamic_cast<const VarDeclNode*>(node)) generateVarDecl(varDecl);
    else if (auto assign = dynamic_cast<const AssignNode*>(node)) generateAssign(assign);
    else if (auto print = dynamic_cast<const PrintNode*>(node)) generatePrint(print);
    else if (auto ifNode = dynamic_cast<const IfNode*>(node)) generateIf(ifNode);
    else if (auto whileNode = dynamic_cast<const WhileNode*>(node)) generateWhile(whileNode);
}

void WasmCodeGenerator::generateVarDecl(const VarDeclNode* node) {
    if (node->value) {
        // WebAssembly locals in this setup are numeric (f64).
        // Skip emitting stack operations for string/text declarations to maintain stack alignment.
        if (node->type == "text") {
            return;
        }
        generateExpr(node->value.get());
        indent(); wat << "local.set $" << node->name << "\n";
    }
}

void WasmCodeGenerator::generateAssign(const AssignNode* node) {
    if (node->value) {
        generateExpr(node->value.get());
        indent(); wat << "local.set $" << node->name << "\n";
    }
}

void WasmCodeGenerator::generatePrint(const PrintNode* node) {
    if (node->expression) {
        generateExpr(node->expression.get());
        indent(); wat << "call $print_num\n";
    }
}

void WasmCodeGenerator::generateIf(const IfNode* node) {
    if (node->condition) generateExpr(node->condition.get());
    
    indent(); wat << "f64.const 0.0\n";
    indent(); wat << "f64.ne\n";
    indent(); wat << "if\n";
    indentLevel++;

    for (const auto& stmt : node->thenBranch) {
        if (stmt) generateNode(stmt.get());
    }

    if (!node->elseBranch.empty()) {
        indentLevel--;
        indent(); wat << "else\n";
        indentLevel++;
        for (const auto& stmt : node->elseBranch) {
            if (stmt) generateNode(stmt.get());
        }
    }

    indentLevel--;
    indent(); wat << "end\n";
}

void WasmCodeGenerator::generateWhile(const WhileNode* node) {
    indent(); wat << "block $break_loop\n";
    indentLevel++;
    indent(); wat << "loop $continue_loop\n";
    indentLevel++;

    if (node->condition) generateExpr(node->condition.get());

    indent(); wat << "f64.const 0.0\n";
    indent(); wat << "f64.eq\n";
    indent(); wat << "br_if $break_loop\n";

    for (const auto& stmt : node->body) {
        if (stmt) generateNode(stmt.get());
    }

    indent(); wat << "br $continue_loop\n";
    indentLevel--;
    indent(); wat << "end\n";
    indentLevel--;
    indent(); wat << "end\n";
}

void WasmCodeGenerator::generateExpr(const ASTNode* expr) {
    if (!expr) return;

    if (auto num = dynamic_cast<const NumberNode*>(expr)) {
        indent(); wat << "f64.const " << num->value << "\n";
    } else if (auto var = dynamic_cast<const VariableNode*>(expr)) {
        indent(); wat << "local.get $" << var->name << "\n";
    } else if (auto bin = dynamic_cast<const BinaryOpNode*>(expr)) {
        generateExpr(bin->left.get());
        generateExpr(bin->right.get());

        std::string op = bin->op;
        if (op == "+") { indent(); wat << "f64.add\n"; }
        else if (op == "-") { indent(); wat << "f64.sub\n"; }
        else if (op == "*") { indent(); wat << "f64.mul\n"; }
        else if (op == "/") { indent(); wat << "f64.div\n"; }
        else if (op == ">") { indent(); wat << "f64.gt\n"; }
        else if (op == "<") { indent(); wat << "f64.lt\n"; }
        else if (op == "==") { indent(); wat << "f64.eq\n"; }
        else if (op == "!=") { indent(); wat << "f64.ne\n"; }
    }
}