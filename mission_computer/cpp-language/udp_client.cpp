#include "udp_client.h"
#include <fstream>
UdpClient::UdpClient(const char* ip, const char* port) {
    _ip = ip;
    _port = port;
	_sock = 0;
	
    memset(&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = inet_addr(_ip);
	_server_addr.sin_port = htons(atoi(_port));

	_server_addr_size = sizeof(_server_addr);
}

UdpClient::~UdpClient() {
    close(_sock);
}

int UdpClient::getSocket() {
    _sock = socket(PF_INET, SOCK_DGRAM, 0);
	return _sock;
}

int UdpClient::requestFile(char* filename) {
	_data_man.openNewfile("filename.csv");
	int recv_len;

	if(_sock == -1 || _sock == 0)
		printf("sock error\n");
	// char *msg = malloc(sizeof(char) * 20);
	// char *first_code = "#$";
	// char *end_code = ".csv%*";

	// strcpy(msg, first_code);
	// strcpy(msg, filename);
	// strcpy(msg, end_code);

	printf("msg: %s\n", filename);

	sendto(_sock, filename, strlen(filename), 0, (struct sockaddr*)&_server_addr, _server_addr_size);

	while(1)
	{
		_data_man.clearBuf(recv_msg);

		recv_len = recvfrom(_sock, recv_msg, BUF_SIZE, 0, (struct sockaddr*)&_server_addr, &_server_addr_size);
		printf("received byte: %d", recv_len);

		if (_data_man.saveFile(recv_msg, recv_len)) {
			break;
		}
	}
	
	return true;
}