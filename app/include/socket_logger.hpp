#pragma once

/*

so since it is required to use nothing but stl,
something like boost asio is not the choise,
and the best guess is to resort to default windows/linux
socket instruments

*/

#include "structs.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib") // link with ws2_32.lib
#endif

#include <iostream>

class SocketLogger {

    public:

    SocketLogger() = default;
    SocketLogger(const std::string&, const unsigned short&);
    ~SocketLogger();

    void waitForClient();
    void write(const std::string&, const common::Priority&);
    void init(const std::string& ip, const unsigned short& port);

    bool hasClient() { return client_socket != INVALID_SOCKET; };
    void closeClient();

    private:

    bool ready = false;
    void setupListeningSocket();

    #ifdef _WIN32
        sockaddr_in server_addr = {};
        WSADATA wsa_data;
        SOCKET listen_socket;
        SOCKET client_socket;
        unsigned short port;
    #endif

};