#pragma once

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib") // link with ws2_32.lib
#endif

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

    // starts up the socket thing, fills the addr info
    void init(const std::string& ip, const unsigned short& port);
    void establishConnection();
    // this one will be getting messages in an infinite loop, since recv is blocking.
    void receiveMessages();
    void closeSocket();
    bool hasConnection() { return connection_socket != INVALID_SOCKET; };
    void createSocket();

    private:

    bool ready = false;
    bool connected = false;

    char recvbuf[512];

    #ifdef _WIN32
        sockaddr_in client_addr = {};
        WSADATA wsa_data;
        SOCKET connection_socket;
        unsigned short port;
    #endif

};