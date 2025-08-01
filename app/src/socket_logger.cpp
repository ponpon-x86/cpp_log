#include "socket_logger.hpp"

#ifdef _WIN32

SocketLogger::SocketLogger(const unsigned short& port) :
port(port), client_socket(INVALID_SOCKET), listen_socket(INVALID_SOCKET) {
    init(port);
}

void SocketLogger::init(const unsigned short& port) {
    if (!ready) {
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
            std::cout << "\tWSAStartup failed.\n";
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        server_addr.sin_port = htons(port);

        setupListeningSocket();
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
}

void SocketLogger::waitForClient() {
    std::cout << "\tWaiting for a client to connect...\n";
    client_socket = accept(listen_socket, nullptr, nullptr);
    if (client_socket == INVALID_SOCKET) {
        std::cout << "\tAccept failed.\n";
    }

    std::cout << "\tClient connected.\n";
}

void SocketLogger::write(const std::string& message, const common::Priority& priority) {
    // i don't think the mutex is needed?
    // std::lock_guard<std::mutex> lock(this->mutex);
    if (!hasClient()) {
        std::cout << "\tAttempted to write to socket, yet there's no client. Aborting.\n";
        return;
    }

    const char* sendbuf = "\tHello! Hello!\n";
    int sendbuflen = strlen(sendbuf);

    if (send(client_socket, sendbuf, sendbuflen, 0) == SOCKET_ERROR) {
        std::cout << "\tSend failed. Closing client.\n";
        closeClient();
    }
}

void SocketLogger::closeClient() {
    if (client_socket != INVALID_SOCKET) {
        closesocket(client_socket);
        client_socket = INVALID_SOCKET;
        std::cout << "\tClosed client's socket.\n";
    }
}

#endif