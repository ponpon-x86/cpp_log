#include "checker.hpp"

Checker::Checker(const int& argc, char** argv) {
    switch (argc) {
        case 1: 
            explaination += "\tError: no command-line arguments provided.\n";
            break;
        case 2:
            explaination += "\tError: not enough command-line arguments provided.\n";
            break;
        case 3:
        {
            // at this point argv[2] should exist
            std::string flag = argv[2];
            // check if the flag is set correctly
            auto dash = flag.front() == '-';
            auto alpha = std::isalpha(flag.back());
            // check if the existing flag is set
            auto indicator = alpha ? common::validatePriority(flag.back()) : false;
            this->passed_checks = (dash && indicator) ? true : false;
            if (!this->passed_checks) explaination += "\tError: provided flag is incorrect.\n";
            break;
        }
        default:
            explaination += "\tError: too many command-line arguments provided.\n";
            break;
    }
    composeMessage();
}

void Checker::composeMessage() {
    explaination += std::string("\tProvide two command-line arguments: one for log file name, one for default priority of entries.\n\n") +
        "\tStartup example: [binary name] log.txt -r\n\n" +
        "\tPossible priority values:\n"
        "\t-r / regular\n"
        "\t-i / important\n"
        "\t-c / critical\n"
        "\tPress [Enter] to quit...\n";
}