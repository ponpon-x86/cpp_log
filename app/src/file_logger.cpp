#include "file_logger.hpp"

FileLogger::FileLogger(const std::string& filename) {
    file.open(filename);
}

FileLogger::~FileLogger() {
    file.close();
}

void FileLogger::write(const std::string& message, const common::Priority& priority) {
    std::lock_guard<std::mutex> lock(this->mutex);
    file << "message: " << message << " [priority: " << common::priorityToString(priority) << "]\n";
}