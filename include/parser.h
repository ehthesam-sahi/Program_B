#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include <memory>

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;

    Token peek() const;
    Token advance();
    bool match(TokenType type);
    bool check(TokenType type) const;
    void synchronize(); // Basic syntax error recovery

    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseVarDecl();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parsePrint();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();

public:
    explicit Parser(const std::vector<Token>& tokenList);
    std::unique_ptr<ProgramNode> parseProgram();
};

#endif