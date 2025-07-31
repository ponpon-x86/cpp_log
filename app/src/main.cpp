#include <iostream>
#include <vector>

#include "app.hpp"

void explanatoryMessage() {
        std::cout << "\tProvide at least two command-line arguments: one for log file name, one for default priority of entries.\n\n";
        std::cout << "\tExample: [binary name] log.txt reg\n\n";
        std::cout << "\tPossible priority values:\n";
        std::cout << "\t- r / regular\n";
        std::cout << "\t- i / important\n";
        std::cout << "\t- c / critical\n";
        std::cout << "\tPress [Enter] to quit...\n";
        getchar();
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "\tError: no command-line arguments provided.\n";
        explanatoryMessage();
        return 2;
    }

    if (argc < 3) {
        std::cout << "\tError: not enough command-line arguments provided.\n";
        explanatoryMessage();
        return 1;
    }

    const std::string log_file_name = argv[1];
    const std::string default_priority = argv[2];
    auto priority_char = default_priority.front();

    App app(log_file_name, common::charToPriority(priority_char));

    std::cout << "Hello world.\n";
    return 0;
}