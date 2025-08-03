#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include <regex>

#include <mutex>
#include <thread>

#include "structs.h"
#include "file_logger.hpp"
#include "socket_logger.hpp"

class Core {

    public:
  
    /*
    it's possible to use constructor initializer list
    */
    Core(const std::string&, const common::Priority&, const std::string& ip = "", const unsigned short& port = 0);
    Core(const common::CoreConfig&, const std::string& ip = "", const unsigned short& port = 0);

    /*
    but also,
    instead of dealing with headaches around constructor initializer list,
    create a default core object, and initialize it later.
    */
    Core() = default;
    void init(const std::string&, const common::Priority&, const std::string& ip = "", const unsigned short& port = 0);
    void init(const common::CoreConfig&, const std::string& ip = "", const unsigned short& port = 0);
    
    ~Core() = default;

    void switchDefaultPriority(const common::Priority&);
    void switchLoggingDestination();

    common::Priority getDefaultPriority();
    bool isLoggingToFile();

    // should be launched in a thread
    void listen();
    // should be launched in a thread
    void pingPong();
    
    // the "send" function
    void log(const std::string&);

    private:

    // doesn't make a lot of sense outside the class, hence
    // declared here
    struct handledInput {
        std::string message;
        std::string priority;
    };
    handledInput handleInput(const std::string&);

    bool initialized = false;
    bool file_logging = true;

    FileLogger file_logger;
    SocketLogger socket_logger;

    const std::string default_ip = "127.0.0.1";
    const short unsigned default_port = 60420;

    common::CoreConfig config;
};