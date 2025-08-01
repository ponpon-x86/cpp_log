#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include <regex>

#include <mutex>
#include <thread>

#include "structs.h"
#include "file_logger.hpp"

class Core {

    public:
  
    /*
    it's possible to use constructor initializer list
    */
    Core(const std::string&, const common::Priority&);
    Core(const common::CoreConfig&);

    /*
    but also,
    instead of dealing with headaches around constructor initializer list,
    create a default core object, and initialize it later.
    */
    Core() = default;
    void init(const std::string&, const common::Priority&);
    void init(const common::CoreConfig&);
    
    ~Core() = default;

    void switchDefaultPriority(const common::Priority&);

    void log(const std::string&);

    private:

    struct handledInput {
        std::string message;
        std::string priority;
    };
    handledInput handleInput(const std::string&);

    bool initialized = false;

    FileLogger fileLogger;
    common::CoreConfig config;
};