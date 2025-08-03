#include "core.hpp"

// constuctors

Core::Core(const std::string& filename, const common::Priority& priority, const std::string& ip, const unsigned short& port) :
file_logger(filename), socket_logger(ip == "" ? default_ip : ip, port == 0 ? default_port : port) {
    this->config = { filename , priority };
    initialized = true;
}

Core::Core(const common::CoreConfig& config, const std::string& ip, const unsigned short& port) :
file_logger(config.log_file_name), socket_logger(ip == "" ? default_ip : ip, port == 0 ? default_port : port) {
    this->config = config;
    initialized = true;
}

// methods
// in case default constructor was used, the core needs to be initialized
void Core::init(const std::string& filename, const common::Priority& priority, const std::string& ip, const unsigned short& port) {
    // just call the function which takes config arg.
    init ( { filename , priority }, ip, port );
}

void Core::init(const common::CoreConfig& config, const std::string& ip, const unsigned short& port) {
    if (initialized) {
        std::cout << "\tThe core is already initialized.\n";
        return;
    }

    this->config = config;
    if(!file_logger.open(config.log_file_name)) {
        std::cout << "\tA file is already open.\n";
    }

    socket_logger.init(ip == "" ? default_ip : ip, port == 0 ? default_port : port);
}

Core::handledInput Core::handleInput(const std::string& input) {
    handledInput handled;

    std::smatch match;
    std::regex regex(R"((.*?)(?:\s+-([a-zA-Z0-9]))?$)");
    // .*? -- capture everything, stop when next token in regex can match

    // \s+-([a-zA-Z0-9]+))?
    // ? -- token is optional
    // \s+ -- any whitespace character (one or unlimited)
    // - -- literally '-'
    // [a-zA-Z0-9] -- match a character
    // $ -- end of the line

    std::string message;
    std::string priority;
    
    if(std::regex_match(input, match, regex)) {
        message = match[1].str();
        if (match[2].matched) {
            priority = match[2].str();
        }
    }

    handled = { message, priority };
    return handled;
}

void Core::listen() {
    socket_logger.waitForClient();
}

void Core::pingPong() {
    socket_logger.pingClient();
}

// log message
void Core::log(const std::string& input) {
    // regex-match the user input
    auto handled = handleInput(input);
    auto validation = handled.priority.empty() ? false : common::validatePriority(handled.priority);

    // if there was no priority provided, just log with default priority
    if (handled.priority.empty()) {

        if(file_logging)
            file_logger.write(handled.message, this->config.priority);
        else socket_logger.write(handled.message, this->config.priority);

        return;
    } 
    
    // if not, then
    // validate whether the set priority is real, 
    auto valid = common::validatePriority(handled.priority);
    if (valid) {
        // and get priority object
        auto p = handled.priority.front();
        auto p_obj = common::charToPriority(p);
        // if validated priority is higher or same as default, write to file
        if (p_obj >= this->config.priority) {
            
            if(file_logging)
                file_logger.write(handled.message, common::charToPriority(p));
            else socket_logger.write(handled.message, common::charToPriority(p));

        }
    }

    // else the message will be discarded -- tough luck
}

// short ones
void Core::switchDefaultPriority(const common::Priority& priority) {
    this->config.priority = priority;
}

void Core::switchLoggingDestination() { 
    file_logging = !file_logging; 
};

common::Priority Core::getDefaultPriority() { 
    return config.priority; 
};

bool Core::isLoggingToFile() { 
    return file_logging; 
};