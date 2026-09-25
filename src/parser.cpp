#include "../include/parser.h"
#include <iostream>
#include <stdexcept>

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

// Error recovery requirement: skip tokens to semicolon or end on syntax error
void Parser::synchronize() {
    advance();
    while (peek().type != TokenType::TOK_EOF) {
        if (tokens[current - 1].type == TokenType::SEMICOLON) return;
        if (peek().type == TokenType::DEFINE || peek().type == TokenType::SET || 
            peek().type == TokenType::WHEN || peek().type == TokenType::REPEAT || 
            peek().type == TokenType::OUTPUT) return;
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
            synchronize(); // Basic syntax error recovery without crashing
        }
    }
    return program;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (check(TokenType::DEFINE)) return parseVarDecl();
    if (check(TokenType::SET)) return parseAssignment();
    if (check(TokenType::OUTPUT)) return parsePrint();
    if (check(TokenType::WHEN)) return parseIf();
    if (check(TokenType::REPEAT)) return parseWhile();

    std::cerr << "Syntax Error at line " << peek().line << ": Unexpected token '" << peek().text << "'\n";
    throw std::runtime_error("Syntax error");
}

std::unique_ptr<ASTNode> Parser::parseVarDecl() {
    match(TokenType::DEFINE);
    Token id = advance(); // Variable name
    match(TokenType::AS);
    Token typeTok = advance(); // Type (number / text)

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

// Parses: when <condition> then <statements> [otherwise <statements>] end;
std::unique_ptr<ASTNode> Parser::parseIf() {
    match(TokenType::WHEN);
    auto node = std::make_unique<IfNode>();
    node->condition = parseExpression();
    match(TokenType::THEN);

    while (!check(TokenType::OTHERWISE) && !check(TokenType::END) && !check(TokenType::TOK_EOF)) {
        node->thenBranch.push_back(parseStatement());
    }

    if (match(TokenType::OTHERWISE)) {
        while (!check(TokenType::END) && !check(TokenType::TOK_EOF)) {
            node->elseBranch.push_back(parseStatement());
        }
    }

    match(TokenType::END);
    match(TokenType::SEMICOLON);
    return node;
}

// Parses: repeat while <condition> do <statements> end;
std::unique_ptr<ASTNode> Parser::parseWhile() {
    match(TokenType::REPEAT);
    match(TokenType::WHILE);
    auto node = std::make_unique<WhileNode>();
    node->condition = parseExpression();

    if (check(TokenType::DO)) match(TokenType::DO);

    while (!check(TokenType::END) && !check(TokenType::TOK_EOF)) {
        node->body.push_back(parseStatement());
    }

    match(TokenType::END);
    match(TokenType::SEMICOLON);
    return node;
}

// Handles relational operators (==, !=, >, <, >=, <=, equals, is_less_than, is_greater_than)
std::unique_ptr<ASTNode> Parser::parseExpression() {
    auto left = parseAdditive();

    while (check(TokenType::EQUALS) || check(TokenType::NOT_EQUALS) ||
           check(TokenType::IS_GREATER_THAN) || check(TokenType::IS_LESS_THAN) ||
           check(TokenType::GREATER_EQUAL) || check(TokenType::LESS_EQUAL)) {
        Token op = advance();
        auto binary = std::make_unique<BinaryOpNode>();
        binary->op = op.text;
        binary->left = std::move(left);
        binary->right = parseAdditive();
        left = std::move(binary);
    }
    return left;
}

// Handles addition and subtraction (+, -)
std::unique_ptr<ASTNode> Parser::parseAdditive() {
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

// Handles multiplication and division (*, /)
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
    if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        match(TokenType::RPAREN);
        return expr;
    }

    std::cerr << "Syntax Error at line " << peek().line << ": Unexpected factor '" << peek().text << "'\n";
    throw std::runtime_error("Unexpected factor");
}