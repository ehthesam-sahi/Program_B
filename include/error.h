#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <iostream>

class ErrorHandler {
public:
    static void logError(const std::string& phase, const std::string& message, int line = 0);
};

#endif