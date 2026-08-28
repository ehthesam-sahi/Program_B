#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

class ASTNode {
public:
    virtual ~ASTNode() = default;
};

class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;
};

class VarDeclNode : public ASTNode {
public:
    std::string name;
    std::string type;
    std::unique_ptr<ASTNode> value;
};

class AssignNode : public ASTNode {
public:
    std::string name;
    std::unique_ptr<ASTNode> value;
};

class PrintNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression;
};

class BinaryOpNode : public ASTNode {
public:
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
};

class NumberNode : public ASTNode {
public:
    double value;
    explicit NumberNode(double val) : value(val) {}
};

class StringNode : public ASTNode {
public:
    std::string value;
    explicit StringNode(std::string val) : value(val) {}
};

class VariableNode : public ASTNode {
public:
    std::string name;
    explicit VariableNode(std::string n) : name(n) {}
};

#endif