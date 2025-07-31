#include "core.hpp"

// constuctors

Core::Core(const std::string& filename, const common::Priority& priority) :
fileLogger(filename) {
    this->config = { filename , priority };
}

Core::Core(const common::CoreConfig& config) :
fileLogger(config.log_file_name) {
    this->config = config;
}

// methods
// change base priority
void Core::switchDefaultPriority(const common::Priority& priority) {
    this->config.priority = priority;
}

// log message (no priority)
void Core::log(const std::string& message) {
    log(message, config.priority);
}

void Core::log(const std::string& message, const common::Priority& priority) {
    auto thread = std::thread(&FileLogger::write, &fileLogger, message, priority);
}