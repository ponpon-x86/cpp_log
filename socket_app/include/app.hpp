#pragma once

#include <atomic>
#include <thread>
#include <mutex>
#include <queue>

#include "checker.hpp"
#include "socket_manager.hpp"
#include "statistician.hpp"

class App {

    public:

    App(const int&, char**);
    ~App();

    private:

    Statistician statistician;
    Checker checker;
    SocketManager socket_manager;

    enum class ConnectionState;

    // the display functions
    void run();
    void info();
    void displayState();
    void statistics();

    // since we need to update after certain amount of time

    // ok this is tricky, since socket manager has to do blocking stuff, 
    // plus it needs to do a lot. the solution i came up with would be
    void socketThreadJob();
    enum class ConnectionState {
        IDLE,
        CREATING_SOCKET,
        CONNECTING,
        CONNECTED,
        FAILED_CREATION,
        FAILED_CONNECTION,
        SOCK_INVALID,
        SOCK_CLOSED,
        SOCK_ERROR,
    };
    std::atomic<ConnectionState> state = ConnectionState::IDLE;

    // thread-safe stuff
    std::thread socket_thead;
    std::mutex mutex;
    std::queue<std::string> message_queue;
    
    std::string last_message;
    std::string last_error;

    // the checker will provide these
    common::NetData net_data;
    std::atomic<long long> messages_left;
    unsigned timer;
};