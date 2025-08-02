#include "socket_manager.hpp"

SocketManager::SocketManager(const std::string& ip, const unsigned short& port) :
connection_socket(INVALID_SOCKET), port(port) {
    init(ip, port);
}

SocketManager::~SocketManager() {
    connected = false;
    shutdown(connection_socket, SD_BOTH);
    closesocket(connection_socket);
    WSACleanup();
}

void SocketManager::createSocket() {
    if (hasConnection()) {
        return;
    }

    std::cout << "\tTrying to create a connection socket...\n";
    connection_socket = INVALID_SOCKET;
    connection_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connection_socket == INVALID_SOCKET) {
        std::cout << "\tFailed to create a socket.\n";
    }
    std::cout << "\tCreated a socket.\n";
}

void SocketManager::init(const std::string& ip, const unsigned short& port) {
    if (!ready) {
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
            std::cout << "\tWSAStartup failed.\n";
        }

        client_addr.sin_family = AF_INET;
        InetPton(AF_INET, ip.c_str(), &client_addr.sin_addr.s_addr);
        client_addr.sin_port = htons(port);

        // createSocket();
    }
    ready = true;
}

void SocketManager::establishConnection() {
    if (connected) {
        return;
    }

    std::cout << "\tTrying to establish connection...\n";
    if (connect(connection_socket, (sockaddr*)&client_addr, sizeof(client_addr) == SOCKET_ERROR)) {
        std::cout << "\tFailed to establish connection.\n";
        connected = false;
    }
    connected = true;
    std::cout << "\tConnection established.\n";
}

void SocketManager::closeSocket() {
    if (connection_socket != INVALID_SOCKET) {
        shutdown(connection_socket, SD_BOTH);
        closesocket(connection_socket);
        connection_socket = INVALID_SOCKET;
        connected = false;
        std::cout << "\tClosed client's socket.\n";
    }
    std::cout << "\tAttempted to close socket, but it's already invalid.\n";
}

void SocketManager::receiveMessages() {
    if (!hasConnection()) {
        std::cout << "\tAttempted to recieve messages, yet the socket is invalid. Aborting.\n";
        return;
    }

    auto recv_result = recv(connection_socket, recvbuf, sizeof(recvbuf), 0);
    if (recv_result == SOCKET_ERROR) {
        std::cout << "\tSocket error:\n";
        std::cout << WSAGetLastError() << "\n";
        closeSocket();
    }
    if (recv_result == 0) {
        std::cout << "\tConnection closed.\n";
        closeSocket();
    }
    std::cout << "\tBytes received: " << recv_result << "\nReceived data: " << recvbuf << "\n";
}