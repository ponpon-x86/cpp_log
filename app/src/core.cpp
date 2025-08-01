#include "core.hpp"

// constuctors

Core::Core(const std::string& filename, const common::Priority& priority) :
fileLogger(filename) {
    this->config = { filename , priority };
    initialized = true;
}

Core::Core(const common::CoreConfig& config) :
fileLogger(config.log_file_name) {
    this->config = config;
    initialized = true;
}

// methods
// in case default constructor was used, the core needs to be initialized
void Core::init(const std::string& filename, const common::Priority& priority) {
    // just call the function which takes config arg.
    init ( { filename , priority } );
}

void Core::init(const common::CoreConfig& config) {
    if (initialized) {
        std::cout << "\tThe core is already initialized.\n";
        return;
    }

    this->config = config;
    if(!fileLogger.open(config.log_file_name)) {
        std::cout << "\tA file is already open.\n";
    }
}

// change default priority
void Core::switchDefaultPriority(const common::Priority& priority) {
    this->config.priority = priority;
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

#include <fstream>
// log message
void Core::log(const std::string& input) {
    auto handled = handleInput(input);
    auto validation = handled.priority.empty() ? false : common::validatePriority(handled.priority);

    if (validation) {
        auto p = handled.priority.front();
        auto p_obj = common::charToPriority(p);

        if (p_obj >= this->config.priority) {
            fileLogger.write(handled.message, common::charToPriority(p));
            std::ofstream file("mogus.txt");
            file << "message: [" << handled.message << "], priority: [" << handled.priority << "]";
            file.close();
        }
    } else {
        fileLogger.write(handled.message, this->config.priority);
        std::ofstream file("focus.txt");
        file << "message: [" << handled.message << "], priority: [" << handled.priority << "]";
        file.close();
    }
}
