#include "../include/symboltable.h"

SymbolTable::SymbolTable() {
    enterScope(); // Global scope
}

void SymbolTable::enterScope() {
    scopes.push_back({});
}

void SymbolTable::exitScope() {
    if (scopes.size() > 1) {
        scopes.pop_back();
    }
}

bool SymbolTable::declareVariable(const std::string& name, const std::string& type, int line) {
    if (isDeclaredInCurrentScope(name)) {
        return false; // Variable already declared in scope
    }
    scopes.back()[name] = Symbol{name, type, true, line};
    return true;
}

bool SymbolTable::isDeclaredInCurrentScope(const std::string& name) const {
    if (scopes.empty()) return false;
    return scopes.back().find(name) != scopes.back().end();
}

bool SymbolTable::isDeclared(const std::string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return true;
        }
    }
    return false;
}

const Symbol* SymbolTable::lookup(const std::string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return &(found->second);
        }
    }
    return nullptr;
}

void SymbolTable::printTable() const {
    std::cout << "\n=== Symbol Table Contents ===\n";
    int scopeLevel = 0;
    for (const auto& scope : scopes) {
        std::cout << "--- Scope Level " << scopeLevel++ << " ---\n";
        for (const auto& pair : scope) {
            std::cout << "  Var: " << pair.first 
                      << " | Type: " << pair.second.type 
                      << " | Declared Line: " << pair.second.lineDeclared << "\n";
        }
    }
    std::cout << "=============================\n";
}