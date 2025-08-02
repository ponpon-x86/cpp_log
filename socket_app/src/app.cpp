#include "app.hpp"

App::App(const int& argc, char** argv) :
checker(argc, argv) {
    if (!checker.allClear()) {
        std::cout << checker.getExplaination();
        getchar();
        return;
    }
    
    // initialize manager
    this->net_data = checker.getNetData();
    auto port_integer = std::stoi(net_data.port);
    socket_manager.init(net_data.ip, port_integer);

    run();
}

App::~App() {
    socket_manager.closeSocket();
}

void App::run() { 
    while (true) {

        info();

        // well first of all
        // the socket manager was already initialized
        // but the socket is not created
        // .createSocket() checks if the socket is already created, so no fear

        socket_manager.createSocket();

        // next the connection should be established, but it's blocking...

        socket_manager.establishConnection();
    }
}

void App::info() {
    std::cout << "\tConnection parameters:\n";
    std::cout << "\tAddress: " << net_data.ip << ":" << net_data.port << "\n";
    std::cout << "\tMessages to statistics update: " << net_data.mtu << "\n";
    std::cout << "\tSeconds to statistics update: " << net_data.timeout << "\n\n";
}