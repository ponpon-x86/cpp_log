#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include <mutex>
#include <thread>

#include "structs.h"
#include "file_logger.hpp"

class Core {

    public:

    Core() = delete;
    
    Core(const std::string&, const common::Priority&);
    Core(const common::CoreConfig&);
    ~Core() = default;

    void switchDefaultPriority(const common::Priority&);

    void log(const std::string&);
    void log(const std::string&, const common::Priority& priority);

    private:

    FileLogger fileLogger;
    common::CoreConfig config;
};