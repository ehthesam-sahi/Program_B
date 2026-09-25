#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/semantic.h"
#include "../include/codegen.h"
#include "../include/codegen_wasm.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::string filePath = "test_programs/valid_test.ps";
    if (argc > 1) {
        filePath = argv[1];
    }

    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        std::cerr << "[Error]: Could not open source file '" << filePath << "'\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string sampleCode = buffer.str();
    inFile.close();

    std::cout << "========================================\n";
    std::cout << "   INVENTED DSL COMPILER DEMO\n";
    std::cout << "========================================\n";
    std::cout << "Reading source file: " << filePath << "\n\n";

    // Pass 1: Tokenization
    std::cout << "--- Pass 1: Tokenization ---\n";
    Lexer lexer(sampleCode);
    auto tokens = lexer.tokenize();

    std::cout << std::left << std::setw(10) << "Index" 
              << std::setw(10) << "Line" 
              << std::setw(20) << "Lexeme" << "\n";
    std::cout << "----------------------------------------\n";
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << std::left << std::setw(10) << (i + 1)
                  << std::setw(10) << tokens[i].line
                  << "[" << tokens[i].text << "]\n";
    }
    std::cout << "\nTotal Generated Tokens: " << tokens.size() << "\n";

    // Pass 2: Parsing & AST
    std::cout << "\n--- Pass 2: Parsing & AST Generation ---\n";
    Parser parser(tokens);
    auto ast = parser.parseProgram();

    if (ast) {
        std::cout << "Parsed " << ast->statements.size() << " top-level statement(s) successfully.\n";
        std::cout << "\n--- Abstract Syntax Tree ---\n";
        ast->printTreeIndented();
    }

    // Pass 3: Semantic Analysis & Type Checking
    std::cout << "\n--- Pass 3: Semantic Analysis & Type Checking ---\n";
    SemanticAnalyzer semanticAnalyzer;
    bool isValid = semanticAnalyzer.analyze(ast.get());
    
    semanticAnalyzer.printErrors();
    semanticAnalyzer.getSymbolTable().printTable();

    if (!isValid) {
        std::cout << "\n[Failed]: Compilation halted due to semantic errors.\n";
        return 1;
    }

    // Pass 4A: Python Code Generation
    std::cout << "\n--- Pass 4A: Target Code Generation (Python) ---\n";
    CodeGenerator pyCodeGen;
    std::string pythonCode = pyCodeGen.generate(ast.get());
    std::cout << pythonCode << "\n";

    std::ofstream pyFile("output/result.py");
    if (pyFile.is_open()) {
        pyFile << pythonCode;
        pyFile.close();
        std::cout << "[Success]: Python code saved to 'output/result.py'\n";
    }

    // Pass 4B: WebAssembly (WAT) Code Generation
    std::cout << "\n--- Pass 4B: Target Code Generation (WebAssembly WAT) ---\n";
    WasmCodeGenerator wasmCodeGen;
    std::string watCode = wasmCodeGen.generate(ast.get());
    std::cout << watCode << "\n";

    std::ofstream watFile("output/result.wat");
    if (watFile.is_open()) {
        watFile << watCode;
        watFile.close();
        std::cout << "[Success]: WebAssembly WAT saved to 'output/result.wat'\n";
    }

    return 0;
}