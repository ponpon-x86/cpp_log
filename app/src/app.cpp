#include "app.hpp"

/*

the original plan was to use a member initializer list,
and initialize core this way.

then i realized i should actually check if the app
was launched correctly, so now a default constuctor of Core
is being fired, followed by init() method in App's constructor

*/

App::App(const int& argc, char** argv) :
checker(argc, argv) {
    if (!checker.allClear()) {
        std::cout << checker.getExplaination();
        getchar();
        return;
    }
    
    // initialize core
    core.init(common::composeConfig(argv[1], argv[2]));
    run();
}

void App::run() {
    std::string input;

    while (running) {
        instructions();

        std::getline(std::cin, input);

        if(commandFilter(input)) continue;

        pool.queue(&Core::log, std::ref(core), input);

        input = "";
    }
}

// this one will return true if input was special;
// special input won't be logged
bool App::commandFilter(const std::string& input) {
    if (input == "-p") {
        specifyPriority();
        return true; // was a special input
    }

    if (input == "-t") {
        core.switchLoggingDestination();
        return true; // was a special input
    }

    if (input == "exit") {
        running = false;
        return true; // was a special input
    }

    return false; // was NOT a special input
}

// override default priority
void App::specifyPriority() {
    std::cout << "\n\tSpecify new default priority [r/i/c]: ";
    std::string new_priority;
    std::getline(std::cin, new_priority);

    if(new_priority.length() != 1) return;

    auto c = new_priority.front();
    auto valid = common::validatePriority(c);

    if (!valid) return;

    core.switchDefaultPriority(common::charToPriority(c));
    return;
}

void App::instructions() {
    // clear screen
    #ifdef _WIN32 // windows
        system("cls");
    #else // will assume unix-based whatever.
        system("clear");
    #endif

    std::cout << "\n";
    std::cout << "\tCurrently logging to: " << (core.isLoggingToFile() ? "file.\n" : "socket.\n");
    std::cout << "\tDefault priority: " << common::priorityToString(core.getDefaultPriority()) << "\n\n";
    std::cout << "\tUnique commands:\n";
    std::cout << "\texit // exit;\n";
    std::cout << "\t-p // change default priority;\n";
    std::cout << "\t-t // switch logging destination (file/socket);\n\n";
    std::cout << "\tGeneral use:\n";
    std::cout << "\t[message] -[priority]\n";
    std::cout << "\tLike so:\n";
    std::cout << "\ta bright red fox jumps over a high fence -r\n";
    std::cout << "\t\tNote: priority is optional.\n\n\t";
}