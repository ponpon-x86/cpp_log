#pragma once

#include <string>
#include <cctype>

#include "structs.h"

class Checker {

    public:

    Checker() = delete;
    Checker(const int&, char**);
    ~Checker() = default;

    const bool allClear() const { return passed_checks; };
    const std::string getExplaination() const { return explaination; };

    private:

    bool passed_checks = false;
    void composeMessage();

    std::string explaination;
};