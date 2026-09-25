#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

struct Symbol {
    std::string name;
    std::string type; // "number" or "text"
    bool isInitialized;
    int lineDeclared;
};

class SymbolTable {
private:
    // Support nested scoping for loops and condition blocks
    std::vector<std::unordered_map<std::string, Symbol>> scopes;

public:
    SymbolTable();

    void enterScope();
    void exitScope();

    bool declareVariable(const std::string& name, const std::string& type, int line);
    bool isDeclared(const std::string& name) const;
    bool isDeclaredInCurrentScope(const std::string& name) const;
    
    const Symbol* lookup(const std::string& name) const;
    void printTable() const;
};

#endif