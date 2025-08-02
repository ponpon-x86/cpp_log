#pragma once

#include <string>
#include <regex>

#include "struct.h"

class Checker {

    public:

    Checker() = delete;
    Checker(const int&, char**);
    ~Checker() = default;

    const bool allClear() const { return passed_checks; };
    const std::string getExplaination() const { return explaination; };
    // the checker checks all this data; might as well store it and use later
    const common::NetData& getNetData() { return net_data; };

    private:

    void examineIpPortString(const std::string&);
    void checkup();
    bool checkIp();
    bool checkPort();

    bool passed_checks = false;
    void composeMessage();

    std::string explaination;
    common::NetData net_data;
};