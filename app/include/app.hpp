#pragma once

#include <vector>

#include "core.hpp"

class App {

    public:

    App(const std::string&, const common::Priority&);
    ~App() = default;

    private:

    void run();

    Core core;

};