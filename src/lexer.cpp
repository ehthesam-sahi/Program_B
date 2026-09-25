#include "../include/lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& sourceCode) : source(sourceCode), pos(0), line(1) {}

char Lexer::peek() const {
    if (pos >= source.length()) return '\0';
    return source[pos];
}

char Lexer::get() {
    if (pos >= source.length()) return '\0';
    char ch = source[pos++];
    if (ch == '\n') {
        line++;
    }
    return ch;
}

void Lexer::skipWhitespace() {
    while (pos < source.length()) {
        char ch = peek();
        if (ch == ' ' || ch == '\t' || ch == '\r') {
            get();
        } else if (ch == '\n') {
            get(); // get() automatically increments line counter
        } else {
            break;
        }
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (pos < source.length()) {
        skipWhitespace();
        if (pos >= source.length()) break;

        char current = peek();

        // Single and Double Character Operators / Symbols
        if (current == ';') { get(); tokens.push_back({TokenType::SEMICOLON, ";", line}); }
        else if (current == '(') { get(); tokens.push_back({TokenType::LPAREN, "(", line}); }
        else if (current == ')') { get(); tokens.push_back({TokenType::RPAREN, ")", line}); }
        else if (current == '{') { get(); tokens.push_back({TokenType::LBRACE, "{", line}); }
        else if (current == '}') { get(); tokens.push_back({TokenType::RBRACE, "}", line}); }
        else if (current == '+') { get(); tokens.push_back({TokenType::PLUS, "+", line}); }
        else if (current == '-') { get(); tokens.push_back({TokenType::MINUS, "-", line}); }
        else if (current == '*') { get(); tokens.push_back({TokenType::MULTIPLY, "*", line}); }
        else if (current == '/') { get(); tokens.push_back({TokenType::DIVIDE, "/", line}); }
        
        // Relational and Equality Operators
        else if (current == '=') {
            get();
            if (peek() == '=') {
                get();
                tokens.push_back({TokenType::EQUALS, "==", line});
            } else {
                tokens.push_back({TokenType::ASSIGN, "=", line});
            }
        }
        else if (current == '!') {
            get();
            if (peek() == '=') {
                get();
                tokens.push_back({TokenType::NOT_EQUALS, "!=", line});
            } else {
                tokens.push_back({TokenType::UNKNOWN, "!", line});
            }
        }
        else if (current == '>') {
            get();
            if (peek() == '=') {
                get();
                tokens.push_back({TokenType::GREATER_EQUAL, ">=", line});
            } else {
                tokens.push_back({TokenType::IS_GREATER_THAN, ">", line});
            }
        }
        else if (current == '<') {
            get();
            if (peek() == '=') {
                get();
                tokens.push_back({TokenType::LESS_EQUAL, "<=", line});
            } else {
                tokens.push_back({TokenType::IS_LESS_THAN, "<", line});
            }
        }

        // Strings
        else if (current == '"') {
            get(); // Consume opening quote
            std::string strVal = "";
            while (peek() != '"' && peek() != '\0') {
                strVal += get();
            }
            if (peek() == '"') {
                get(); // Consume closing quote
            }
            tokens.push_back({TokenType::STRING_LITERAL, strVal, line});
        }

        // Numbers (Integers and Floating Points)
        else if (std::isdigit(current)) {
            std::string numStr = "";
            while (std::isdigit(peek()) || peek() == '.') {
                numStr += get();
            }
            tokens.push_back({TokenType::NUMBER_LITERAL, numStr, line});
        }

        // Keywords and Identifiers
        else if (std::isalpha(current) || current == '_') {
            std::string ident = "";
            while (std::isalnum(peek()) || peek() == '_') {
                ident += get();
            }

            if (ident == "define") tokens.push_back({TokenType::DEFINE, ident, line});
            else if (ident == "as") tokens.push_back({TokenType::AS, ident, line});
            else if (ident == "with") tokens.push_back({TokenType::WITH, ident, line});
            else if (ident == "set") tokens.push_back({TokenType::SET, ident, line});
            else if (ident == "to") tokens.push_back({TokenType::TO, ident, line});
            else if (ident == "when") tokens.push_back({TokenType::WHEN, ident, line});
            else if (ident == "then") tokens.push_back({TokenType::THEN, ident, line});
            else if (ident == "otherwise") tokens.push_back({TokenType::OTHERWISE, ident, line});
            else if (ident == "repeat") tokens.push_back({TokenType::REPEAT, ident, line});
            else if (ident == "while") tokens.push_back({TokenType::WHILE, ident, line});
            else if (ident == "do") tokens.push_back({TokenType::DO, ident, line});
            else if (ident == "end") tokens.push_back({TokenType::END, ident, line});
            else if (ident == "output") tokens.push_back({TokenType::OUTPUT, ident, line});
            else if (ident == "number") tokens.push_back({TokenType::TYPE_NUMBER, ident, line});
            else if (ident == "text") tokens.push_back({TokenType::TYPE_TEXT, ident, line});
            else if (ident == "equals") tokens.push_back({TokenType::EQUALS, ident, line});
            else if (ident == "is_less_than") tokens.push_back({TokenType::IS_LESS_THAN, ident, line});
            else if (ident == "is_greater_than") tokens.push_back({TokenType::IS_GREATER_THAN, ident, line});
            else tokens.push_back({TokenType::IDENTIFIER, ident, line});
        }
        else {
            std::string unk(1, get());
            tokens.push_back({TokenType::UNKNOWN, unk, line});
        }
    }

    tokens.push_back({TokenType::TOK_EOF, "", line});
    return tokens;
}