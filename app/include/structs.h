#pragma once

#include <string>

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
        switch(c) {
            case 'r':
                return Priority::REGULAR;
            case 'i':
                return Priority::IMPORTANT;
            case 'c':
                return Priority::CRITICAL;
        }
    }

}