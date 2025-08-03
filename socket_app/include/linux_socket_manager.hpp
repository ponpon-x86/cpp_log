#pragma once
#ifndef _WIN32

#include <iostream>
#include <mutex>
#include <string>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

class SocketManager {

    public:

    SocketManager() = default;
    SocketManager(const std::string& ip, const unsigned short& port);
    ~SocketManager();

    enum class SocketOperationResult {
        EXISTS,
        SUCCESS,
        FAILURE
    };

    enum class RecvResult {
        INVALID_S,
        S_ERROR,
        C_CLOSED,
        SUCCESS,
        PING
    };

    // starts up the socket thing, fills the addr info
    bool init(const std::string& ip, const unsigned short& port);
    SocketOperationResult establishConnection();
    // first is a flag, second is the message
    std::pair<RecvResult, std::string> receiveMessages();
    void closeSocket();
    bool hasSocket() { return connection_socket != INVALID_SOCKET; };
    bool hasConnection() { return connected; };
    SocketOperationResult createSocket();

    private:

    bool ready = false;
    bool connected = false;

    char pongbuf[512] = {0}; // init with null bytes
    const int message_length = 512;

    sockaddr_in client_addr = {};
    int connection_socket;
    
    std::string ip;
    unsigned short port;

    const int INVALID_SOCKET = -1;
    const int SOCKET_ERROR = -1;

};

#endif