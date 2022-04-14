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

string UdpClient::encodeMsg(string filename)
{
	string str_msg;
	string start_byte = "#$";
	string end_byte = ".csv%*";

	str_msg = start_byte + filename + end_byte;

	return str_msg;
}

bool UdpClient::requestFile(string filename) 
{
	if(!_data_man.openNewfile(filename)) {
		cout << "already exist file" << endl;
		return 0;
	}

	if(_sock == -1 || _sock == 0) {
		printf("sock error\n");
		return 0;
	}

	string str_msg = encodeMsg(filename);
	const char* send_msg = str_msg.c_str();
	cout << send_msg << endl;

	int recv_len;

	sendto(_sock, send_msg, strlen(send_msg), 0, (struct sockaddr*)&_server_addr, _server_addr_size);

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