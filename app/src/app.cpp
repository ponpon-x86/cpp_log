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

        pool.queue(&Core::log, &core, input);

        input = "";
    }
}

const void App::instructions() const {
    // clear screen
    #ifdef _WIN32 // windows
        system("cls");
    #else // will assume unix-based whatever.
        system("clear");
    #endif

    std::cout << "\n";
    std::cout << "\tCurrently logging to: " << (file_logging ? "file.\n\n" : "socket.\n\n");
    std::cout << "\tUnique commands:\n";
    std::cout << "\t-p // change default priority;\n";
    std::cout << "\t-t // switch logging destination (file/socket);\n\n";
    std::cout << "\tGeneral use:\n";
    std::cout << "\t[message] -[priority]\n";
    std::cout << "\tLike so:\n";
    std::cout << "\ta bright red fox jumps over a high fence -r\n";
    std::cout << "\t\tNote: priority is optional.\n\n\t";
}