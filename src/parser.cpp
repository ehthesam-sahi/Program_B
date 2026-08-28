#include "../include/parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokenList) : tokens(tokenList), current(0) {}

Token Parser::peek() const { return tokens[current]; }

Token Parser::advance() {
    if (peek().type != TokenType::TOK_EOF) current++;
    return tokens[current - 1];
}

bool Parser::check(TokenType type) const { return peek().type == type; }

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

// Error recovery requirement: skip tokens to semicolon on syntax error
void Parser::synchronize() {
    advance();
    while (peek().type != TokenType::TOK_EOF) {
        if (tokens[current - 1].type == TokenType::SEMICOLON) return;
        advance();
    }
}

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>();
    while (!check(TokenType::TOK_EOF)) {
        try {
            auto stmt = parseStatement();
            if (stmt) program->statements.push_back(std::move(stmt));
        } catch (...) {
            synchronize(); // Basic syntax error recovery
        }
    }
    return program;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (check(TokenType::DEFINE)) return parseVarDecl();
    if (check(TokenType::SET)) return parseAssignment();
    if (check(TokenType::OUTPUT)) return parsePrint();

    std::cerr << "Syntax Error at line " << peek().line << ": Unexpected token '" << peek().text << "'\n";
    throw std::runtime_error("Syntax error");
}

std::unique_ptr<ASTNode> Parser::parseVarDecl() {
    match(TokenType::DEFINE);
    Token id = advance(); // Variable name
    match(TokenType::AS);
    Token typeTok = advance(); // Type

    auto node = std::make_unique<VarDeclNode>();
    node->name = id.text;
    node->type = typeTok.text;

    if (match(TokenType::WITH)) {
        node->value = parseExpression();
    }
    match(TokenType::SEMICOLON);
    return node;
}

std::unique_ptr<ASTNode> Parser::parseAssignment() {
    match(TokenType::SET);
    Token id = advance();
    match(TokenType::TO);
    auto node = std::make_unique<AssignNode>();
    node->name = id.text;
    node->value = parseExpression();
    match(TokenType::SEMICOLON);
    return node;
}

std::unique_ptr<ASTNode> Parser::parsePrint() {
    match(TokenType::OUTPUT);
    match(TokenType::LPAREN);
    auto node = std::make_unique<PrintNode>();
    node->expression = parseExpression();
    match(TokenType::RPAREN);
    match(TokenType::SEMICOLON);
    return node;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    auto left = parseTerm();
    while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
        Token op = advance();
        auto binary = std::make_unique<BinaryOpNode>();
        binary->op = op.text;
        binary->left = std::move(left);
        binary->right = parseTerm();
        left = std::move(binary);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseTerm() {
    auto left = parseFactor();
    while (check(TokenType::MULTIPLY) || check(TokenType::DIVIDE)) {
        Token op = advance();
        auto binary = std::make_unique<BinaryOpNode>();
        binary->op = op.text;
        binary->left = std::move(left);
        binary->right = parseFactor();
        left = std::move(binary);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseFactor() {
    if (check(TokenType::NUMBER_LITERAL)) {
        return std::make_unique<NumberNode>(std::stod(advance().text));
    }
    if (check(TokenType::STRING_LITERAL)) {
        return std::make_unique<StringNode>(advance().text);
    }
    if (check(TokenType::IDENTIFIER)) {
        return std::make_unique<VariableNode>(advance().text);
    }
    throw std::runtime_error("Unexpected factor");
}