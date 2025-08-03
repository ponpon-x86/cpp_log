#include "statistician.hpp"

Statistician::Statistician() {
    message_max_length.old = message_max_length.current = -1;
    message_min_length.old = message_min_length.current = -1;
    message_average_length.old = message_average_length.current = -1;
}

void Statistician::update() {
    messages_total.update();
    for (auto& m : messages_priority)
        m.update();
    messages_last_hour.update();
    message_max_length.update();
    message_min_length.update();
    message_average_length.update();
}

#include <iostream>
#include <fstream>

void Statistician::newMessage(const std::string& message) {
    // first match should be "message: " text
    // second should be message body
    // third would be the text " (priority: "
    // so we need the fourth one
    ++messages_since_update;

    // i guess i'll use "body" as message length,
    // since nothing in task mentions what counts as message length
    std::string body;
    std::string priority;
    if(std::regex_match(message, match, regex)) {
        body = match[2].str();
        priority = match[4].str();
    }

    // now to... assign things
    ++messages_total.current;
    std::ofstream debug("ultra.txt");
    debug << "message str: " << message << "\n";
    debug << "message: " << "[" << body << "]" << "[" << priority << "]" << "\n";
    debug.close();
    //exit(1);
    if (priority == "regular") ++messages_priority[Priority::REGULAR].current;
    if (priority == "important") ++messages_priority[Priority::IMPORTANT].current;
    if (priority == "critical") ++messages_priority[Priority::CRITICAL].current;

    timestamps.push_back(Clock::now());
    messages_last_hour.current = timestamps.size();

    auto body_length = body.length();
    setLengths(body_length);
}

void Statistician::removeExpired() {
    auto now = Clock::now();
    // basically, while front of timestamps, in hours, is more than an hour away,
    while (
        std::chrono::duration_cast<std::chrono::hours>(now - timestamps.front()).count() >= 1
    ) {
        timestamps.pop_front();
    }
    messages_last_hour.current = timestamps.size();
}

void Statistician::setLengths(const int& len) {
    if (
        message_max_length.current == -1 ||
        message_max_length.current < len
    ) {
        message_max_length.current = len;
    }

    if (
        message_min_length.current == -1 ||
        message_min_length.current > len
    ) {
        message_min_length.current = len;
    }

    if (message_average_length.current == -1) {
        message_average_length.current = len;
    }

    total_length_of_all_messages += static_cast<long long>(len);
    message_average_length.current = total_length_of_all_messages / messages_total.current;
}

void Statistician::startClock() {
    if (!started) resetClock();
    started = true;
    return;
}

void Statistician::resetClock() { 
    start_time = end_time = Clock::now(); 
}

long long Statistician::getClockDif() {
    return std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
}

bool Statistician::shouldUpdate(long long& mtu) {
    if (messages_since_update >= mtu) {
        messages_since_update = 0;
        return true;
    }
    return false;
}

bool Statistician::shouldUpdate(long long& mtu, long long& timeout) { 
    bool should = false;
    should = shouldUpdate(mtu);

    end_time = Clock::now();

    if (timeout - getClockDif() <= 0) {
        resetClock();
        should = true;
    }
    return should;
}