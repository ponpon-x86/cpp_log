#pragma once

#include <array>
#include <string>
#include <chrono>
#include <deque>
#include <regex>

/*

    there are six things to keep track of:

    - total number of messages
    - total number of messages by priority
    - total number of messages in last hour
    - minimum length of a message
    - maximum length of a message
    - average length of a message

*/

class Statistician {

    public:

    Statistician();
    ~Statistician() = default;
    void update();

    // so back when the original app was written, there was declared
    // an enum of priorities there. problem is, -strings- are being sent via socket;
    // while it is possible to use that same enum here as well, we could just
    // inspect the string with regex and find priority that way with no fuss
    void newMessage(const std::string& message);

    // since these are just get functions, i'll define them here
    long long getTotalMessagesCount() {
        return messages_total.old;
    }
    std::tuple<long long, long long, long long> 
    getPriorityMessagesCount() { 
        return std::make_tuple(
            messages_priority[Priority::REGULAR].old,
            messages_priority[Priority::IMPORTANT].old,
            messages_priority[Priority::CRITICAL].old
        ); 
    }
    long long getLastHourMessagesCount() {
        return messages_last_hour.old;
    }
    long long getMaxMessageLength() {
        return message_max_length.old;
    }
    long long getMinMessageLength() {
        return message_min_length.old;
    }
    long long getAverageMessageLength() {
        return message_average_length.old;
    }
    
    bool shouldUpdateByMessages(long long& mtu);
    bool shouldUpdateByTimeout(long long& timeout);

    void startClock();
    long long getClockDif();

    private:

    bool started = false;

    void removeExpired(); // can be called in update()
    void setLengths(const int&);
    void resetClock();

    struct statistic {
        long long old = 0;
        long long current = 0;

        void update() {
            old = current;
        }
    };
    
    enum Priority { 
        REGULAR = 0, 
        IMPORTANT = 1, 
        CRITICAL = 2,
        COUNT
    };
    
    // self explanatory, just total messages
    statistic messages_total;
    // and this will contain number of messages by priority
    std::array<statistic, Priority::COUNT> messages_priority;

    // it could be pair of tracker struct and a vector, but
    // let them be separate
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    std::deque<TimePoint> timestamps;
    statistic messages_last_hour;

    // also self explanatory
    statistic message_max_length;
    statistic message_min_length;

    // will explain later
    long long total_length_of_all_messages = 0;
    statistic message_average_length;

    // once again i turn to my crippling addiction
    std::smatch match;
    std::regex regex = std::regex(R"((message:\s)(.*)( \s*\(priority:\s*)([a-zA-Z]+)(.*\s*$))");

    // our clock
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point end_time;

    long long messages_since_update = 0;
};