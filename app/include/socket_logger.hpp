#pragma once
#ifdef _WIN32

/*

so since it is required to use nothing but stl,
something like boost asio is not the choise,
and the best guess is to resort to default windows/linux
socket instruments

*/

#include "structs.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // link with ws2_32.lib

#include <iostream>
#include <mutex>

class SocketLogger {

    public:

    SocketLogger() = default;
    SocketLogger(const std::string&, const unsigned short&);
    ~SocketLogger();

    // non-blocking
    void waitForClient();
    void write(const std::string&, const common::Priority&);
    void init(const std::string& ip, const unsigned short& port);

    /*
        it's impossible to rely on sends to determine whether
        the client disconnected, so we'll poke them with a stick
    */
    void pingClient();

    private:

    bool ready = false;
    void setupListeningSocket();
    bool hasClient() { 
        // fine, i'll put mutex on it
        std::lock_guard<std::mutex> lock(mutex);
        return client_socket != INVALID_SOCKET; 
    };
    void closeClient();

    sockaddr_in server_addr = {};
    WSADATA wsa_data;
    SOCKET listen_socket;
    SOCKET client_socket;
    unsigned short port;

    char pingbuf[512] = {0}; // init with null bytes
    const int message_length = 512;

    std::mutex mutex;

};

#endif