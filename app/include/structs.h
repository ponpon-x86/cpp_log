#pragma once

#include <string>
#include <ctime>
#include <algorithm>

namespace common {

    enum class Priority {
        REGULAR,
        IMPORTANT,
        CRITICAL
    };

    struct CoreConfig {
        std::string log_file_name;
        Priority priority;
    };

    inline std::string priorityToString(Priority priority) {
        switch(priority) {
            case Priority::REGULAR:
                return "regular";
            case Priority::IMPORTANT:
                return "important";
            case Priority::CRITICAL:
                return "critical";
            default:
                return "unknown";
        }
    }

    inline Priority charToPriority(const char& c) {
        switch(std::tolower(c)) {
            case 'r':
                return Priority::REGULAR;
            case 'i':
                return Priority::IMPORTANT;
            case 'c':
                return Priority::CRITICAL;
        }
    }

    inline CoreConfig composeConfig(char* file_arg, char* priority_arg) {
        const std::string log_file_name = file_arg;
        const std::string default_priority = priority_arg;
        // only the last character is needed
        auto priority_char = default_priority.back();
        // convert the character to priority
        auto priority = common::charToPriority(priority_char);
        // return config object
        return { log_file_name , priority };
    }

    inline std::string getTime() {
        std::time_t current_time = std::time(nullptr);
        std::tm* local_time = std::localtime(&current_time);

        auto year = local_time->tm_year + 1900;
        auto month = local_time->tm_mon + 1;
        auto day = local_time->tm_mday;
        auto hours = local_time->tm_hour;
        auto minutes = local_time->tm_min;
        auto seconds = local_time->tm_sec;

        std::string result;
        result = 
            std::to_string(hours) + ":" +
            std::to_string(minutes) + ":" +
            std::to_string(seconds) + " " +
            std::to_string(day) + "/" +
            std::to_string(month) + "/" +
            std::to_string(year);

        return result;
    }

    inline std::string stringToLowerCase(std::string& string) {
        std::transform(string.begin(), string.end(), string.begin(), ::tolower);
    }

    inline bool validatePriority(char p) {
        switch(std::tolower(p)) {
            case 'r':
            case 'i':
            case 'c':
            return true;
            default: return false;
        }
    }

    inline bool validatePriority(std::string priority) {
        auto p = priority.front();
        return validatePriority(p);
    }
}