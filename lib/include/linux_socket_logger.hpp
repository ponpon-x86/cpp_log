#pragma once
#ifndef _WIN32

#include "structs.h"

#include <iostream>
#include <mutex>
#include <string>
#include <string.h>

/*
windows needs
<winsock2.h> and <ws2tcpip.h>, 
linux needs these:
*/

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

class SocketLogger {
    public:

    SocketLogger() = default;
    SocketLogger(const std::string&, const unsigned short&);
    ~SocketLogger();

    // non-blocking
    common::socket::ClientWaitResult waitForClient();
    common::socket::WriteResult write(const std::string&, const common::Priority&);
    void init(const std::string& ip, const unsigned short& port);
    bool isInited() { return ready; }
    
    /*
        it's impossible to rely on sends to determine whether
        the client disconnected, so we'll poke them with a stick
    */
    common::socket::PingResult pingClient();

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
    // winsock uses SOCKET data type, linux uses int
    int listen_socket;
    int client_socket;
    unsigned short port;

    char pingbuf[512] = {0}; // init with null bytes
    const int message_length = 512;

    // invalid socket in linux is -1
    const int INVALID_SOCKET = -1;
    // socket error is seemingly also -1
    const int SOCKET_ERROR = -1;

    std::mutex mutex;

};

#endif