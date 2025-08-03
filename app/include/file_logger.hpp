#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <mutex>

#include "structs.h"

class FileLogger {

    public:

    FileLogger() = default;
    FileLogger(const std::string& filename);
    ~FileLogger();

    // if false, the file is already opened
    bool open(const std::string& filename);

    void write(
        const std::string& message, 
        const common::Priority& priority);

    private:

    std::mutex mutex;
    std::ofstream file;
};