#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

enum class TokenType {
    // Keywords
    DEFINE, AS, WITH, SET, TO, WHEN, THEN, OTHERWISE, REPEAT, WHILE, DO, OUTPUT,
    TYPE_NUMBER, TYPE_TEXT,

    // Relational Operators
    EQUALS, NOT_EQUALS, IS_LESS_THAN, IS_GREATER_THAN, LE, GE,

    // Arithmetic / Operators
    PLUS, MINUS, MULTIPLY, DIVIDE, ASSIGN,

    // Literals & Identifiers
    IDENTIFIER, NUMBER_LITERAL, STRING_LITERAL,

    // Delimiters
    SEMICOLON, LPAREN, RPAREN, LBRACE, RBRACE,

    // End of File / Unknown
    TOK_EOF, UNKNOWN
};

struct Token {
    TokenType type;
    std::string text;
    int line;
};

class Lexer {
private:
    std::string source;
    size_t pos;
    int line;

    char peek() const;
    char get();
    void skipWhitespace();

public:
    explicit Lexer(const std::string& sourceCode);
    std::vector<Token> tokenize();
};

#endif