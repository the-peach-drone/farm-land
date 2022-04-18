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
        UdpClient(string ip, int port);
        ~UdpClient();

        int getSocket();
        bool requestFile(string filename);
        string encodeMsg(string filename);
        bool requestHistory(int last_few_days);

        int gettodaydate(int eve_offset);

    private:
        char recv_msg[BUF_SIZE];
        struct sockaddr_in _server_addr;
        socklen_t _server_addr_size;

        string _ip;
        int _port;

        int _sock;

        DataManagement _data_man;
};

#endif