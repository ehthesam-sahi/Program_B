#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>

// Helper for 2D grid rendering
struct TreeLineBuffer {
    std::vector<std::string> lines;

    void ensureCell(size_t r, size_t c) {
        if (r >= lines.size()) lines.resize(r + 1);
        if (c >= lines[r].size()) lines[r].resize(c + 1, ' ');
    }

    void putChar(size_t r, size_t c, char ch) {
        ensureCell(r, c);
        lines[r][c] = ch;
    }

    void putStr(size_t r, size_t c, const std::string& str) {
        ensureCell(r, c + str.size());
        for (size_t i = 0; i < str.size(); ++i) {
            lines[r][c + i] = str[i];
        }
    }
};

struct TreeRenderResult {
    size_t width;
    size_t rootX;
};

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string getNodeLabel() const = 0;
    virtual std::vector<const ASTNode*> getChildren() const = 0;

    // Standard Indented Text Tree (Guaranteed to fit any terminal width)
    virtual void printTreeIndented(const std::string& prefix = "", bool isLast = true) const {
        std::cout << prefix;
        std::cout << (isLast ? "└── " : "├── ");
        std::cout << getNodeLabel() << "\n";

        auto children = getChildren();
        for (size_t i = 0; i < children.size(); ++i) {
            if (children[i]) {
                bool lastChild = (i == children.size() - 1);
                std::string newPrefix = prefix + (isLast ? "    " : "│   ");
                children[i]->printTreeIndented(newPrefix, lastChild);
            }
        }
    }

    // 2D Graphical Tree
    TreeRenderResult renderSubtree(TreeLineBuffer& buf, size_t row, size_t col) const {
        std::string label = "[" + getNodeLabel() + "]";
        size_t labelLen = label.length();
        auto children = getChildren();

        if (children.empty()) {
            buf.putStr(row, col, label);
            return {labelLen, col + labelLen / 2};
        }

        std::vector<TreeRenderResult> childResults;
        size_t currentChildCol = col;
        size_t childRow = row + 3;

        for (size_t i = 0; i < children.size(); ++i) {
            if (i > 0) currentChildCol += 2;
            if (children[i]) {
                auto res = children[i]->renderSubtree(buf, childRow, currentChildCol);
                childResults.push_back(res);
                currentChildCol += res.width;
            }
        }

        if (childResults.empty()) {
            buf.putStr(row, col, label);
            return {labelLen, col + labelLen / 2};
        }

        size_t firstRootX = childResults.front().rootX;
        size_t lastRootX = childResults.back().rootX;
        size_t parentX = (firstRootX + lastRootX) / 2;

        size_t subtreeWidth = std::max(labelLen, currentChildCol - col);
        size_t labelStartCol = (parentX >= labelLen / 2) ? (parentX - labelLen / 2) : 0;
        buf.putStr(row, labelStartCol, label);

        buf.putChar(row + 1, parentX, '|');

        if (childResults.size() == 1) {
            buf.putChar(row + 2, parentX, '|');
        } else {
            for (size_t x = firstRootX; x <= lastRootX; ++x) {
                buf.putChar(row + 2, x, '-');
            }
            buf.putChar(row + 2, parentX, '+');
            for (const auto& res : childResults) {
                buf.putChar(row + 2, res.rootX, '+');
            }
        }

        return {subtreeWidth, parentX};
    }

    void printTree() const {
        TreeLineBuffer buf;
        renderSubtree(buf, 0, 0);
        for (const auto& line : buf.lines) {
            std::cout << line << "\n";
        }
    }
};

class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    std::string getNodeLabel() const override {
        return "ProgramNode";
    }

    std::vector<const ASTNode*> getChildren() const override {
        std::vector<const ASTNode*> list;
        for (const auto& stmt : statements) {
            if (stmt) list.push_back(stmt.get());
        }
        return list;
    }
};

class VarDeclNode : public ASTNode {
public:
    std::string name;
    std::string type;
    std::unique_ptr<ASTNode> value;

    std::string getNodeLabel() const override {
        return "VarDecl: " + name + " (" + type + ")";
    }

    std::vector<const ASTNode*> getChildren() const override {
        std::vector<const ASTNode*> list;
        if (value) list.push_back(value.get());
        return list;
    }
};

class AssignNode : public ASTNode {
public:
    std::string name;
    std::unique_ptr<ASTNode> value;

    std::string getNodeLabel() const override {
        return "Assign: " + name;
    }

    std::vector<const ASTNode*> getChildren() const override {
        std::vector<const ASTNode*> list;
        if (value) list.push_back(value.get());
        return list;
    }
};

class PrintNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression;

    std::string getNodeLabel() const override {
        return "PrintNode";
    }

    std::vector<const ASTNode*> getChildren() const override {
        std::vector<const ASTNode*> list;
        if (expression) list.push_back(expression.get());
        return list;
    }
};

class IfNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> thenBranch;
    std::vector<std::unique_ptr<ASTNode>> elseBranch;

    std::string getNodeLabel() const override {
        return "IfNode (when)";
    }

    std::vector<const ASTNode*> getChildren() const override {
        std::vector<const ASTNode*> list;
        if (condition) list.push_back(condition.get());
        for (const auto& stmt : thenBranch) {
            if (stmt) list.push_back(stmt.get());
        }
        for (const auto& stmt : elseBranch) {
            if (stmt) list.push_back(stmt.get());
        }
        return list;
    }
};

class WhileNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> body;

    std::string getNodeLabel() const override {
        return "WhileNode (repeat while)";
    }

    std::vector<const ASTNode*> getChildren() const override {
        std::vector<const ASTNode*> list;
        if (condition) list.push_back(condition.get());
        for (const auto& stmt : body) {
            if (stmt) list.push_back(stmt.get());
        }
        return list;
    }
};

class BinaryOpNode : public ASTNode {
public:
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    std::string getNodeLabel() const override {
        return "BinaryOp: " + op;
    }

    std::vector<const ASTNode*> getChildren() const override {
        std::vector<const ASTNode*> list;
        if (left) list.push_back(left.get());
        if (right) list.push_back(right.get());
        return list;
    }
};

class NumberNode : public ASTNode {
public:
    double value;
    explicit NumberNode(double val) : value(val) {}

    std::string getNodeLabel() const override {
        std::string str = std::to_string(value);
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        if (str.back() == '.') str.pop_back();
        return "Num: " + str;
    }

    std::vector<const ASTNode*> getChildren() const override {
        return {};
    }
};

class StringNode : public ASTNode {
public:
    std::string value;
    explicit StringNode(std::string val) : value(std::move(val)) {}

    std::string getNodeLabel() const override {
        return "String: \"" + value + "\"";
    }

    std::vector<const ASTNode*> getChildren() const override {
        return {};
    }
};

class VariableNode : public ASTNode {
public:
    std::string name;
    explicit VariableNode(std::string n) : name(std::move(n)) {}

    std::string getNodeLabel() const override {
        return "Var: " + name;
    }

    std::vector<const ASTNode*> getChildren() const override {
        return {};
    }
};

#endif