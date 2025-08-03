#include "app.hpp"

App::App(const int& argc, char** argv) :
checker(argc, argv) {
    if (!checker.allClear()) {
        std::cout << checker.getExplaination();
        getchar();
        return;
    }
    
    // initialize manager
    this->net_data = checker.getNetData();

    auto port_integer = std::stoi(net_data.port);
    this->messages_left = std::stoi(net_data.mtu);
    this->timer = std::stoi(net_data.timeout);

    if (!socket_manager.init(net_data.ip, port_integer)) {
        std::cout << "\tFailed to init socket manager. Aborting.\n";
        return;
    }

    run();
}

App::~App() {
    socket_manager.closeSocket();
}

void App::socketThreadJob() {
    while (true) {
        // well first of all
        // the socket manager was already initialized
        // but the socket is not created
        // .createSocket() checks if the socket is already created, so no fear
        state = socket_manager.hasSocket() ? ConnectionState::CONNECTED : ConnectionState::CREATING_SOCKET;
        auto create_result = socket_manager.createSocket();
        if (create_result == SocketManager::SocketOperationResult::FAILURE) {
            state = ConnectionState::FAILED_CREATION;
            // the loop is tight with just "continue", so... let's sleep the thread.
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        // next the connection should be established, but it's blocking.
        // luckily we are in a thread
        state = socket_manager.hasConnection() ? ConnectionState::CONNECTED : ConnectionState::CONNECTING;
        auto connect_result = socket_manager.establishConnection();
        if (connect_result == SocketManager::SocketOperationResult::FAILURE) {
            state = ConnectionState::FAILED_CONNECTION;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        state = ConnectionState::CONNECTED;
        // this should block the loop
        // and the result is, well, .first() is a flag,
        // .second() is the message.
        auto result = socket_manager.receiveMessages();
        auto flag = result.first;
        auto message = result.second;

        // if this was ping, it's just that we were bothered is all
        if (flag == SocketManager::RecvResult::PING)
            continue;
        // if everything's alright, push the message
        if (flag == SocketManager::RecvResult::SUCCESS) {
            {
                std::unique_lock<std::mutex> lock(mutex);
                statistician.newMessage(message);
                message_queue.push(message);
                continue;
            }
        }

        // if not
        // state what's wrong
        if (flag == SocketManager::RecvResult::INVALID_S) {
            state = ConnectionState::SOCK_INVALID;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        if (flag == SocketManager::RecvResult::C_CLOSED) {
            state = ConnectionState::SOCK_CLOSED;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        if (flag == SocketManager::RecvResult::S_ERROR) {
            last_error = message;
            state = ConnectionState::SOCK_ERROR;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
    }
}

void App::run() { 
    socket_thead = std::thread(&App::socketThreadJob, this);
    statistician.startClock();
    #ifdef _WIN32 // windows
        system("cls");
    #endif

    while (true) {
        info();
        displayState();
        statistics();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void App::info() {
    // clear screen
    #ifdef _WIN32 // windows
        // system("cls");
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE); // Get a handle to the console output
        COORD position = {0, 0}; // Define the desired cursor coordinates
        SetConsoleCursorPosition(hStdout, position);
    #else // will assume unix-based whatever.
        system("clear");
    #endif

    std::cout << "\n\tConnection parameters:\n";
    std::cout << "\33[2K\r" << "\tTrying to connect to: " << net_data.ip << ":" << net_data.port << "\n";
    std::cout << "\33[2K\r" << "\tMessages to statistics update: " << messages_left << " (set to " << net_data.mtu << " messages)\n";
    std::cout << "\33[2K\r" << "\tSeconds to statistics update: " << std::stoi(this->net_data.timeout) - statistician.getClockDif() << " (set to " << net_data.timeout << "s)\n\n";
    std::cout << "\tLast entry:\n";
    {
        std::unique_lock<std::mutex> lock(mutex);
        if(!message_queue.empty()) {
            last_message = message_queue.front();
            message_queue.pop();

            auto numerical_check = --messages_left;
            if(statistician.shouldUpdate(numerical_check))
                statistician.update();
        }
    }
    std::cout << "\33[2K\r" << "\t" << last_message << "\n";
}

void App::displayState() {
    std::cout << "\tConnection state:\n";
    std::cout << "\33[2K\r" << "\t > ";
    switch (state)
    {
        case ConnectionState::IDLE:
            std::cout << "idle";
            break;
        case ConnectionState::CREATING_SOCKET:
            std::cout << "creating socket...";
            break;
        case ConnectionState::CONNECTING:
            std::cout << "connecting...";
            break;
        case ConnectionState::CONNECTED:
            std::cout << "connected (" << this->net_data.ip << ":" << this->net_data.port << ")";
            break;
        case ConnectionState::FAILED_CREATION:
            std::cout << "failed at socket creation; sleeping...";
            break;
        case ConnectionState::FAILED_CONNECTION:
            std::cout << "failed at connection; sleeping...";
            break;
        case ConnectionState::SOCK_INVALID:
            std::cout << "failed to receive message (socket is invalid); sleeping...";
            break;
        case ConnectionState::SOCK_CLOSED:
            std::cout << "failed to receive message (socket is closed); sleeping...";
            break;
        case ConnectionState::SOCK_ERROR:
            std::cout << "failed to receive message (error); sleeping...";
            break;
    }
    std::cout << "\n\n";
}

void App::statistics() {
    std::cout << "\t-----------------------------------------\n";
    std::cout << "\tSTATISTICS\n";
    std::cout << "\t-----------------------------------------\n\n";

    std::cout << "\tTotal messages: " << statistician.getTotalMessagesCount() << "\n";
    auto priority_messages = statistician.getPriorityMessagesCount();
    std::cout << "\tTotal regular messages: " << std::get<0>(priority_messages) << "\n";
    std::cout << "\tTotal importaint messages: " << std::get<1>(priority_messages) << "\n";
    std::cout << "\tTotal critical messages: " << std::get<2>(priority_messages) << "\n";
    std::cout << "\tTotal messages in last hour: " << statistician.getLastHourMessagesCount() << "\n";
    
    auto max = statistician.getMaxMessageLength();
    auto min = statistician.getMinMessageLength();
    auto average = statistician.getAverageMessageLength();

    std::cout << "\33[2K\r" << "\tMax message length: " << (max < 0 ? "[no messages yet]" : std::to_string(max)) << "\n";
    std::cout << "\33[2K\r" << "\tMin message length: " << (min < 0 ? "[no messages yet]" : std::to_string(min)) << "\n";
    std::cout << "\33[2K\r" << "\tAverage message length: " << (average < 0 ? "[no messages yet]" : std::to_string(average)) << "\n\n";

    auto timeout = std::stoi(this->net_data.timeout);
    auto long_timeout = static_cast<long long>(timeout);
    
    auto mtu = std::stoi(this->net_data.mtu);
    auto long_mtu = static_cast<long long>(mtu);

    if (statistician.shouldUpdate(long_mtu, long_timeout)) {
        statistician.update();
    }
}