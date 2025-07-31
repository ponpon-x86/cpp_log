#pragma once

#include <string>
#include <fstream>
#include <mutex>

#include "structs.h"

class FileLogger {

    public:

    FileLogger() = delete;
    FileLogger(const std::string& filename);
    ~FileLogger();

    void write(
        const std::string& message, 
        const common::Priority& priority);

    private:

    std::mutex mutex;
    std::ofstream file;
};