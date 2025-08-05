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
    bool isFileOpen() { return file.is_open(); }

    // returns false on fail
    bool write(
        const std::string& message, 
        const common::Priority& priority);

    void close() { file.close(); }

    private:

    std::mutex mutex;
    std::ofstream file;
};