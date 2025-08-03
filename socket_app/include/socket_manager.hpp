#pragma once
#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // link with ws2_32.lib

#include <iostream>
#include <string>

/*

    it's not required to reconnect, so init everything and connect once,
    then handle recv

*/

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
    WSADATA wsa_data;
    SOCKET connection_socket;
    std::string ip;
    unsigned short port;

};

#endif