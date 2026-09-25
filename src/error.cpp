#include "../include/error.h"

void ErrorHandler::logError(const std::string& phase, const std::string& message, int line) {
    std::cerr << "[" << phase << " Error]";
    if (line > 0) {
        std::cerr << " Line " << line;
    }
    std::cerr << ": " << message << "\n";
}