#include "file_logger.hpp"

FileLogger::FileLogger(const std::string& filename) {
    file.open(filename);
}

FileLogger::~FileLogger() {
    file.close();
}

bool FileLogger::open(const std::string& filename) {
    if (!file.is_open()) {
        file.open(filename);
        return true;
    }
    return false;
}

void FileLogger::write(const std::string& message, const common::Priority& priority) {
    std::lock_guard<std::mutex> lock(this->mutex);
    file << "message: " << message << " (priority: " << common::priorityToString(priority) << ") [" << common::getTime() << "]\n";
    file.flush();
}