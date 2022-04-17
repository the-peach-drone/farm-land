#include "udp_client.h"
#include <fstream>
#include <time.h>

UdpClient::UdpClient(string ip, int port) {
	_ip = ip;
	_port = port;
	_sock = 0;
	
	memset(&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = inet_addr(_ip.c_str());
	_server_addr.sin_port = htons(_port);
}

UdpClient::~UdpClient() {
	close(_sock);
}

int UdpClient::getSocket() {
    _sock = socket(PF_INET, SOCK_DGRAM, 0);
	struct timeval optVal = {3, 0};
	int optLen = sizeof(optVal);
	setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, &optVal, optLen); //timeout 3s
	return _sock;
}

string UdpClient::encodeMsg(string filename)
{
	string str_msg;
	string start_byte = "#$";
	string end_byte = ".csv%************************************************\n";

	str_msg = start_byte + filename + end_byte;

	return str_msg;
}

int UdpClient::gettodaydate(int eve_offset){

    int ldate;
    time_t clock;
    struct tm *date;
    clock = time(0) - (24 * 60 * 60 * eve_offset);
    date = localtime(&clock);

    ldate = date->tm_year * 100000;
    ldate += (date->tm_mon + 1) * 1000;
    ldate += date->tm_mday * 10;
    ldate += date->tm_wday;
    ldate += 190000000;
    ldate /= 10;

    return ldate;
}

bool UdpClient::requestHistory() { //last few days
    int day1 = gettodaydate(2);
    int day2 = gettodaydate(1);
    int day3 = gettodaydate(0);

    printf("day1: %d\n", day1);
    printf("day2: %d\n", day2);
    printf("day3: %d\n", day3);

    bool day1_ret = false;
    bool day2_ret = false;
    bool day3_ret = false;

    while(!day1_ret || !day3_ret || !day3_ret) {
        while(!day1_ret)
            day1_ret = requestFile(to_string(day1));
        printf("save day1\n");
        sleep(2);

        while(!day2_ret)
            day2_ret = requestFile(to_string(day2));
        printf("save day2\n");
        sleep(2);
        
        while(!day3_ret)
            day3_ret = requestFile(to_string(day3));
        printf("save day3\n");
        sleep(2);
    }

    return true;
}

bool UdpClient::requestFile(string filename) 
{
	bool requestResult = false;

	if(!_data_man.openNewfile(filename)) {
		cout << "already exist file" << endl;
		return requestResult;
	}

	if(_sock == -1 || _sock == 0) {
		printf("sock error\n");
		return requestResult;
	}

	string str_msg = encodeMsg(filename);
	const char* send_msg = str_msg.c_str();

	int recv_len = 1;
	
	while(recv_len != -1)
		recv_len = recvfrom(_sock, recv_msg, BUF_SIZE, MSG_DONTWAIT, (struct sockaddr*)&_server_addr, &_server_addr_size);

	sendto(_sock, send_msg, strlen(send_msg), 0, (struct sockaddr*)&_server_addr, sizeof(_server_addr));

	while(1)
	{
		_server_addr_size = sizeof(_server_addr);
		_data_man.clearBuf(recv_msg);

		recv_len = recvfrom(_sock, recv_msg, BUF_SIZE, 0, (struct sockaddr*)&_server_addr, &_server_addr_size);
		if(recv_len < 0)
		{
			printf("timeout\n");
			_data_man.closeFile();
			requestResult = false;
			break;
		}

		if (_data_man.saveFile(recv_msg, recv_len)) {
			requestResult = true;
			break;
		}
	}
	
	return requestResult;
}