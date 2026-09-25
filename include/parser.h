#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include <memory>
#include <vector>

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;

    Token peek() const;
    Token advance();
    bool match(TokenType type);
    bool check(TokenType type) const;
    void synchronize(); // Basic syntax error recovery (skips to semicolon or statement boundary)

    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseVarDecl();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parsePrint();
    std::unique_ptr<ASTNode> parseIf();        // Handles when ... then ... otherwise ... end;
    std::unique_ptr<ASTNode> parseWhile();     // Handles repeat while ... do ... end;
    
    // Operator Precedence Hierarchy
    std::unique_ptr<ASTNode> parseExpression(); // Relational/Comparison operators
    std::unique_ptr<ASTNode> parseAdditive();   // Addition & Subtraction (+, -)
    std::unique_ptr<ASTNode> parseTerm();       // Multiplication & Division (*, /)
    std::unique_ptr<ASTNode> parseFactor();     // Literals, Variables, Parenthesized Exprs

public:
    explicit Parser(const std::vector<Token>& tokenList);
    std::unique_ptr<ProgramNode> parseProgram();
};

#endif