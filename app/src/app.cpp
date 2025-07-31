#include "app.hpp"

App::App(const std::string& log_file_name, const common::Priority& default_priority) : 
core(log_file_name, default_priority) {
    run();
}