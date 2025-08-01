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

    const void instructions() const;

    Pool pool;
    Checker checker;
    Core core;

    bool running = true;
    bool file_logging = true;
};