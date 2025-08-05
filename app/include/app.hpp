#pragma once

#include <vector>
#include <cstdlib>
#include <regex>

#include "pool.hpp"
#include "core.hpp"
#include "checker.hpp"

class App {

    public:

    App(const int&, char**);
    ~App() = default;

    private:

    void run();
    void stop();
    bool commandFilter(const std::string&);

    void instructions();

    // these call correlating core methods
    void specifyPriority();

    void listenJob();
    void pingPongJob();
    std::thread listen_thread;
    std::thread ping_pong_thread;

    Pool pool;
    Checker checker;
    Core core;

    bool running = true;
    bool pinging = true;
    bool listening = true;

    std::condition_variable cv;
    // just so we are clear which one is it
    std::mutex ping_pong_mutex;
};