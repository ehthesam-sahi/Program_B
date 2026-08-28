#include <iostream>
#include "../include/lexer.h"
#include "../include/parser.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    // Force Windows Terminal / PowerShell to render UTF-8 box-drawing characters
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::string sampleCode = 
        "define total as number with 10 + 5 * 2;\n"
        "output(total);";

    std::cout << "--- Tokenizing ---\n";
    Lexer lexer(sampleCode);
    auto tokens = lexer.tokenize();

    for (const auto& tok : tokens) {
        std::cout << "Token Line " << tok.line << ": " << tok.text << "\n";
    }

    std::cout << "\n--- Parsing ---\n";
    Parser parser(tokens);
    auto ast = parser.parseProgram();

    std::cout << "Successfully parsed " << ast->statements.size() << " top-level statement(s).\n";

    if (ast) {
        std::cout << "\n--- Parse Tree ---\n";
        ast->printTree();
    }

    return 0;
}