#include <iostream>
#include <vector>

#include "checker.hpp"
#include "app.hpp"
#include "socket_logger.hpp"

int main(int argc, char *argv[]) {
    // App app(argc, argv);

    SocketLogger logger(60420);
    while(true) {
        if (!logger.hasClient()) {
            std::cout << "Waiting for client.\n";
            logger.waitForClient();
        }
        else {
            logger.write("message", common::Priority::REGULAR);
            Sleep(3000);
        }
    }
    return 0;
}