#include "socket_manager.hpp"

SocketManager::SocketManager(const std::string& ip, const unsigned short& port) :
connection_socket(INVALID_SOCKET), ip(ip), port(port) {
    init(ip, port);
}

SocketManager::~SocketManager() {
    connected = false;
    shutdown(connection_socket, SD_BOTH);
    closesocket(connection_socket);
    WSACleanup();
}

SocketManager::SocketOperationResult
SocketManager::createSocket() {
    if (hasSocket()) {
        return SocketOperationResult::EXISTS;
    }

    // std::cout << "\tTrying to create a connection socket...\n";
    connection_socket = INVALID_SOCKET;
    connection_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connection_socket == INVALID_SOCKET) {
        // std::cout << "\tFailed to create a socket.\n";
        return SocketOperationResult::FAILURE;
    }
    // std::cout << "\tCreated a socket.\n";
    return SocketOperationResult::SUCCESS;
}

bool SocketManager::init(const std::string& ip, const unsigned short& port) {
    if (!ready) {
        connection_socket = INVALID_SOCKET;
        this->ip = ip; this->port = port;

        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
            // std::cout << "\tWSAStartup failed.\n";
            return false;
        }

        client_addr.sin_family = AF_INET;
        InetPton(AF_INET, ip.c_str(), &client_addr.sin_addr.s_addr);
        client_addr.sin_port = htons(port);

        // createSocket();
    }
    ready = true;
    return true;
}

SocketManager::SocketOperationResult
 SocketManager::establishConnection() {
    if (connected) {
        return SocketOperationResult::EXISTS;
    }

    // std::cout << "\tTrying to establish connection...\n";
    if (connect(connection_socket, (sockaddr*)&client_addr, sizeof(client_addr)) == SOCKET_ERROR) {
        // std::cout << "\tFailed to establish connection.\n";
        connected = false;
        return SocketOperationResult::FAILURE;
    }
    connected = true;
    // std::cout << "\tConnection established.\n";
    return SocketOperationResult::SUCCESS;
}

void SocketManager::closeSocket() {
    if (connection_socket != INVALID_SOCKET) {
        shutdown(connection_socket, SD_BOTH);
        closesocket(connection_socket);
        connection_socket = INVALID_SOCKET;
        connected = false;
        // std::cout << "\tClosed client's socket.\n";
    }
    // std::cout << "\tAttempted to close socket, but it's already invalid.\n";
}

std::pair<SocketManager::RecvResult, std::string>
SocketManager::receiveMessages() {
    if (!hasSocket()) {
        // std::cout << "\tAttempted to recieve messages, yet the socket is invalid. Aborting.\n";
        return { RecvResult::INVALID_S , "" } ;
    }

    auto recv_result = recv(connection_socket, recvbuf, sizeof(recvbuf), 0);
    if (recv_result == SOCKET_ERROR) {
        closeSocket();
        std::string error = std::to_string(WSAGetLastError());
        return { RecvResult::S_ERROR , error } ;
    }
    if (recv_result == 0) {
        // std::cout << "\tConnection closed.\n";
        closeSocket();
        return { RecvResult::C_CLOSED , "" } ;
    }
    recvbuf[recv_result] = '\0';
    std::string message = recvbuf;
    // std::cout << "\tBytes received: " << recv_result << "\nReceived data: " << recvbuf << "\n";
    return { RecvResult::SUCCESS , message } ;
}