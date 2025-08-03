#include "checker.hpp"

Checker::Checker(const int& argc, char** argv) {
    switch (argc) {
        case 1: 
            explaination += "\tError: no command-line arguments provided.\n";
            break;
        case 2:
            explaination += "\tError: not enough command-line arguments provided.\n";
            break;
        case 4:
        {
            // this means there's also ip:port
            std::string ip_port = argv[3];
            if (!checkIpPort(ip_port)) {
                this->passed_checks = false;
                return;
            }
            // then let it fall through into case 3, since it also applies
        }
        case 3:
        {
            // at this point argv[2] should exist
            std::string flag = argv[2];
            if(!checkDefault(flag)) {
                this->passed_checks = false;
                explaination += "\tError: provided flag is incorrect.\n";
                return;
            }

            this->passed_checks = true;
            break;
        }
        default:
            explaination += "\tError: too many command-line arguments provided.\n";
            break;
    }
    composeMessage();
}

bool Checker::checkIpPort(const std::string& ip_port) {
    std::smatch match;
    std::regex regex(R"(([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)\:([0-9]+)$)");

    std::string ip;
    std::string port;
    
    if(std::regex_match(ip_port, match, regex)) {
        ip = match[1].str();
        port = match[2].str();
    }

    if (ip == "" || port == "") {
        explaination += "\tError: ip:port set incorrectly.\n";
        return false;
    }

    auto port_integer = std::stoi(port);
    if (port_integer < 0 || port_integer > 65535) {
        explaination += "\tError: port is out of range.\n";
        return false;
    }

    regex = std::regex(R"(([0-9]+)\.([0-9]+)\.([0-9]+)\.([0-9]+)$)");

    std::vector<std::string> ip_numbers;
    if(std::regex_match(ip, match, regex)) {
        ip_numbers.push_back(match[1].str());
        ip_numbers.push_back(match[2].str());
        ip_numbers.push_back(match[3].str());
        ip_numbers.push_back(match[4].str());
    }
    for (auto& n : ip_numbers) {
        if (n.empty()) {
            explaination += "\tError: ip is set incorrectly.\n";
            return false;
        }
        auto integer = std::stoi(n);
        if (integer < 0 || integer > 255) {
            explaination += "\tError: ip is set incorrectly (out of range).\n";
            return false;
        }
    }

    return true;
}

bool Checker::checkDefault(const std::string flag) {
    // check if the flag is set correctly
    auto dash = flag.front() == '-';
    auto alpha = std::isalpha(flag.back());
    // check if the existing flag is set
    auto indicator = alpha ? common::validatePriority(flag.back()) : false;
    return (dash && indicator) ? true : false;
}

void Checker::composeMessage() {
    explaination += std::string("\tProvide at least two command-line arguments: one for log file name, one for default priority of entries.\n") +
        "\tOptional third argument is ip:port\n" +
        "\tIf it's not set up, it defaults to 127.0.0.1:60420\n\n" +
        "\tStartup example: [binary name] log.txt -r 127.0.0.1:60420\n\n" +
        "\tPossible priority values:\n" +
        "\t-r / regular\n" +
        "\t-i / important\n" +
        "\t-c / critical\n" +
        "\tPress [Enter] to quit...\n";
}