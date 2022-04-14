#ifndef _UDP_CLIENT_H
#define _UDP_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "data_management.h"

class UdpClient {
    public:
        UdpClient(const char* ip, const char* port);
        ~UdpClient();
        int getSocket();
        int requestFile(char* file_name);

    private:
        char recv_msg[BUF_SIZE];
        struct sockaddr_in _server_addr;
        socklen_t _server_addr_size;

        const char* _ip;
        const char* _port;

        int _sock;

        DataManagement _data_man;
};

#endif