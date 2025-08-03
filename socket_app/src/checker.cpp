#include "checker.hpp"

Checker::Checker(const int& argc, char** argv) {
    // there should be... 3 arguments
    // ip:port
    // amount of messages when stats are updated
    // timeout till stats are updated (in seconds)

    // it's going to be similar to app's checker.

    switch (argc) {
        case 1: 
            explaination += "\tError: no command-line arguments provided.\n";
            break;
        case 2:
        case 3:
            explaination += "\tError: not enough command-line arguments provided.\n";
            break;
        case 4:
        {
            std::string ip_port = argv[1];
            this->net_data.mtu = argv[2];
            this->net_data.timeout = argv[3]; 
            examineIpPortString(ip_port);
            checkup();
            break;
        }
        default:
            explaination += "\tError: too many command-line arguments provided.\n";
            break;
    }
    composeMessage();
}

void Checker::composeMessage() {
    explaination += std::string("\tProvide three command-line arguments:\n") +
        "\t > first for ip::port to connect to;\n" +
        "\t > second for the amount of messages until app's update;\n" +
        "\t > third for timeout (in seconds) until update.\n\n" +
        "\tStartup example: [binary name] 127.0.0.1:60420 3 5\n\n" +
        "\tPress [Enter] to quit...\n";
}

void Checker::examineIpPortString(const std::string& ip_port) {
    // regex time! this time for an ip addr.
    // this should cover it
    std::smatch match;
    std::regex regex(R"(([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)\:([0-9]+)$)");

    std::string ip;
    std::string port;
    
    if(std::regex_match(ip_port, match, regex)) {
        ip = match[1].str();
        port = match[2].str();
    }
    // now, if regex didn't match, it SHOULD be fine, --
    // just have to check whether ip and port are not empty

    this->net_data.ip = ip;
    this->net_data.port = port;
}

bool Checker::checkIp() {
    // regex...
    std::smatch match;
    std::regex regex(R"(([0-9]+)\.([0-9]+)\.([0-9]+)\.([0-9]+)$)");

    std::vector<std::string> ip_numbers;
    
    if(std::regex_match(this->net_data.ip, match, regex)) {
        ip_numbers.push_back(match[1].str());
        ip_numbers.push_back(match[2].str());
        ip_numbers.push_back(match[3].str());
        ip_numbers.push_back(match[4].str());
    }

    // i want to check if matched, just in case

    for (auto& n : ip_numbers) {
        if (n.empty()) {
            explaination += "\tError: ip failed to match its regex.\n";
            return false;
        }
    }

    // now check the range

    if (!common::inIpRange(ip_numbers)) return false;
    return true;
}

bool Checker::checkPort() {
    auto integer = std::stoi(this->net_data.port);
    return common::inPortRange(integer);
}

void Checker::checkup() {
    // check whether ip and port matched in regex.
    if (this->net_data.somethingIsEmpty()) {
        this->passed_checks = false; 
        explaination += "\tError: some of the arguments turned up empty.\n";
        if (this->net_data.emptyIpPort()) 
            explaination += "\tFor one, ip or port turned up empty. This could be due to ip:port arg string being passed incorretly.\n";
            explaination += "\tMake sure your ip:port argument looks like this (example):\n";
            explaination += "\t127.0.0.1:60420\n\n";
        return;
    }

    // sighhh so we can check the ip and port,
    // general rule being that in ip X.X.X.X each X is between 0 and 255
    if (!checkIp()) { this->passed_checks = false; return; }
    // and ports range from 0 to 65535
    if (!checkPort()) { this->passed_checks = false; return; }

    // ...now check if the remaining arguments are number and we are good to go.
    // these should be digits
    if (
        common::containsOnlyDigits(this->net_data.mtu) &&
        common::containsOnlyDigits(this->net_data.timeout)
    ) { this->passed_checks = true; return; }

    this->passed_checks = false; 
    return;
}