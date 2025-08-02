#pragma once

#include "checker.hpp"
#include "socket_manager.hpp"

class App {

    public:

    App(const int&, char**);
    ~App();

    private:

    Checker checker;
    SocketManager socket_manager;

    void run();
    void info();
    void statistics();

    common::NetData net_data;
    
};