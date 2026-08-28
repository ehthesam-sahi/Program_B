#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void printTree(const std::string& indent = "", bool isLast = true) const = 0;
};

class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    void printTree(const std::string& indent = "", bool isLast = true) const override {
        std::cout << indent << (isLast ? "└── " : "├── ") << "ProgramNode" << std::endl;
        std::string childIndent = indent + (isLast ? "    " : "│   ");
        for (size_t i = 0; i < statements.size(); ++i) {
            bool last = (i == statements.size() - 1);
            if (statements[i]) {
                statements[i]->printTree(childIndent, last);
            }
        }
    }
};

class VarDeclNode : public ASTNode {
public:
    std::string name;
    std::string type;
    std::unique_ptr<ASTNode> value;

    void printTree(const std::string& indent = "", bool isLast = true) const override {
        std::cout << indent << (isLast ? "└── " : "├── ") << "VarDeclNode (" << name << " : " << type << ")" << std::endl;
        if (value) {
            std::string childIndent = indent + (isLast ? "    " : "│   ");
            value->printTree(childIndent, true);
        }
    }
};

class AssignNode : public ASTNode {
public:
    std::string name;
    std::unique_ptr<ASTNode> value;

    void printTree(const std::string& indent = "", bool isLast = true) const override {
        std::cout << indent << (isLast ? "└── " : "├── ") << "AssignNode (" << name << ")" << std::endl;
        if (value) {
            std::string childIndent = indent + (isLast ? "    " : "│   ");
            value->printTree(childIndent, true);
        }
    }
};

class PrintNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression;

    void printTree(const std::string& indent = "", bool isLast = true) const override {
        std::cout << indent << (isLast ? "└── " : "├── ") << "PrintNode" << std::endl;
        if (expression) {
            std::string childIndent = indent + (isLast ? "    " : "│   ");
            expression->printTree(childIndent, true);
        }
    }
};

class BinaryOpNode : public ASTNode {
public:
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    void printTree(const std::string& indent = "", bool isLast = true) const override {
        std::cout << indent << (isLast ? "└── " : "├── ") << "BinaryOpNode (" << op << ")" << std::endl;
        std::string childIndent = indent + (isLast ? "    " : "│   ");
        if (left) left->printTree(childIndent, false);
        if (right) right->printTree(childIndent, true);
    }
};

class NumberNode : public ASTNode {
public:
    double value;
    explicit NumberNode(double val) : value(val) {}

    void printTree(const std::string& indent = "", bool isLast = true) const override {
        std::cout << indent << (isLast ? "└── " : "├── ") << "NumberNode (" << value << ")" << std::endl;
    }
};

class StringNode : public ASTNode {
public:
    std::string value;
    explicit StringNode(std::string val) : value(val) {}

    void printTree(const std::string& indent = "", bool isLast = true) const override {
        std::cout << indent << (isLast ? "└── " : "├── ") << "StringNode (\"" << value << "\")" << std::endl;
    }
};

class VariableNode : public ASTNode {
public:
    std::string name;
    explicit VariableNode(std::string n) : name(n) {}

    void printTree(const std::string& indent = "", bool isLast = true) const override {
        std::cout << indent << (isLast ? "└── " : "├── ") << "VariableNode (" << name << ")" << std::endl;
    }
};

#endif