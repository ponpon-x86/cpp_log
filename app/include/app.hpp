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
    bool commandFilter(const std::string&);

    void instructions();

    // these call correlating core methods
    void specifyPriority();

    Pool pool;
    Checker checker;
    Core core;

    bool running = true;
};