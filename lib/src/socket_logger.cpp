#include "socket_logger.hpp"
#ifdef _WIN32

SocketLogger::SocketLogger(const std::string& ip, const unsigned short& port) :
port(port), client_socket(INVALID_SOCKET), listen_socket(INVALID_SOCKET) {
    init(ip, port);
}

void SocketLogger::init(const std::string& ip, const unsigned short& port) {
    if (!ready) {
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
            std::cout << "\tWSAStartup failed.\n";
        }

        server_addr.sin_family = AF_INET;
        InetPton(AF_INET, ip.c_str(), &server_addr.sin_addr.s_addr);
        server_addr.sin_port = htons(port);
        
        client_socket = INVALID_SOCKET;
        listen_socket = INVALID_SOCKET;

        setupListeningSocket();

        // secret sause
        strcpy(pingbuf, "PING");
    }
    ready = true;
}

SocketLogger::~SocketLogger() {
    closesocket(listen_socket);
    closesocket(client_socket);
    WSACleanup();
}

void SocketLogger::setupListeningSocket() {
    std::cout << "\tTrying to setup on port " << port << "...\n";

    listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket == INVALID_SOCKET) {
        std::cout << "\tListen socket creation failed.\n";
    }
    
    if (bind(listen_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cout << "\tBind failed.\n";
    }

    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "\tListen failed.\n";
    }

    u_long mode = 1;
    ioctlsocket(listen_socket, FIONBIO, &mode); // set to non-blocking
}

common::socket::ClientWaitResult SocketLogger::waitForClient() {
    if (hasClient()) {
        // std::cout << "\tAlready has a client. No need to wait for them.\n";
        return common::socket::ClientWaitResult::HAS_CLIENT;
    }

    // std::cout << "\tWaiting for a client to connect...\n";
    {
        std::unique_lock<std::mutex> lock(mutex);
        client_socket = accept(listen_socket, nullptr, nullptr);
        if (client_socket == INVALID_SOCKET) {
            // there's this "error", WSAEWOULDBLOCK, which is not an error, really
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK) {
                return common::socket::ClientWaitResult::NO_CONNECTION_YET;
            } else {
                return common::socket::ClientWaitResult::FAILURE;
            }

            return common::socket::ClientWaitResult::FAILURE;
        }
    }

    return common::socket::ClientWaitResult::SUCCESS;
}

common::socket::PingResult SocketLogger::pingClient() {
    if (!hasClient()) {
        return common::socket::PingResult::NO_CLIENT;
    }
    // so since write() will be running in another thread, i have a strong
    // desire to mutex this whole thing, so that client_socket won't
    // suddenly explode or something
    std::lock_guard<std::mutex> lock(mutex);

    // first, ping the client
    // "hey, are you still here?"
    send(client_socket, pingbuf, sizeof(pingbuf), 0);

    // we'll need the file descriptor set for select
    // since i don't want to wait in case recv blocks
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(client_socket, &readfds);
    
    // set a 3 second timeout: if nothing comes back from the client
    // within this window, we'll assume they are DEAD.
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    // client_socket + 1 so that select checks descriptors from 0 to client_socket
    auto select_result = select(client_socket + 1, &readfds, NULL, NULL, &timeout);

    // socket marked as ready to read and select is positive??
    if (select_result > 0 && FD_ISSET(client_socket, &readfds)) {
        // we read the data from client
        char recvbuf[512] = {0};
        auto recv_result = recv(client_socket, recvbuf, sizeof(recvbuf), 0);
        if (recv_result <= 0) {
            // client disconnected
            closeClient();
            return common::socket::PingResult::CLIENT_DISCONNECTED;
        }
        std::string message = recvbuf;
        if (message == "PONG") {
            // the client is alive
            return common::socket::PingResult::CLIENT_ALIVE;
        } else {
            // the client is not supposed to send anything to us
            // kill him
            closeClient();
            return common::socket::PingResult::TERMINATED;
        }
    } else {
        // timeout
        // they might as well be dead
        closeClient();
        return common::socket::PingResult::TIMEOUT;
    }
}

common::socket::WriteResult SocketLogger::write(const std::string& message, const common::Priority& priority) {
    if (!hasClient()) {
        // std::cout << "\tAttempted to write to socket, yet there's no client. Aborting.\n";
        return common::socket::WriteResult::NO_CLIENT;
    }

    std::string str = "message: " + message + " (priority: " + common::priorityToString(priority) + ") [" + common::getTime() + "]\n";
    
    // safely copy that to buffer
    char sendbuf[512] = {0};
    auto chars_to_copy = ((str.length()) < (message_length - 1)) ? (str.length()) : (message_length - 1);
    str.copy(sendbuf, chars_to_copy, 0);
    // just in case
    sendbuf[chars_to_copy] = '\0';

    {
        std::unique_lock<std::mutex> lock(mutex);
        if(send(client_socket, sendbuf, sizeof(sendbuf), 0) == SOCKET_ERROR) {
            std::cout << "\tSend failed. Closing client.\n";
            closeClient();
            return common::socket::WriteResult::FAILURE;
        }
    }

    return common::socket::WriteResult::SUCCESS;
}

void SocketLogger::closeClient() {
    if (client_socket != INVALID_SOCKET) {
        closesocket(client_socket);
        client_socket = INVALID_SOCKET;
        std::cout << "\tClosed client's socket.\n";
    }
}

#endif