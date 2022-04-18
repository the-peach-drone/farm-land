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
	struct timeval optVal = {5, 0};
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

int UdpClient::getOtherDay(int eve_offset){

    int ldate;
    time_t clock;
    struct tm *date;
    clock = _utc - (24 * 60 * 60 * eve_offset);
	cout << clock << endl;
    date = localtime(&clock);

    ldate = date->tm_year * 100000;
    ldate += (date->tm_mon + 1) * 1000;
    ldate += date->tm_mday * 10;
    ldate += date->tm_wday;
    ldate += 190000000;
    ldate /= 10;

    return ldate;
}

int UdpClient::getDayToUtc() {

	int year, month, day;
    year = _today / 10000;
    month = (_today -(year *10000)) / 100;
    day = _today % 100;

    struct tm t = {0};
    t.tm_year = year - 1900;
    t.tm_mon = month-1;
    t.tm_mday = day;
	_utc = mktime(&t);

    return mktime(&t);
}

bool UdpClient::requestToday() {
	const char* send_msg = "#!today********************************************************\n";
	bool requestResult = false;
	int recv_len = 0;
	
	while(recv_len != -1)
		recv_len = recvfrom(_sock, recv_msg, BUF_SIZE, MSG_DONTWAIT, (struct sockaddr*)&_server_addr, &_server_addr_size);

	sendto(_sock, send_msg, strlen(send_msg), 0, (struct sockaddr*)&_server_addr, sizeof(_server_addr));

	_server_addr_size = sizeof(_server_addr);
	_data_man.clearBuf(recv_msg);

	recv_len = recvfrom(_sock, recv_msg, BUF_SIZE, 0, (struct sockaddr*)&_server_addr, &_server_addr_size);

	if(recv_len < 0)
	{
		printf("timeout\n");
		requestResult = false;
	}
	else {
		requestResult = saveDate(recv_msg, recv_len);
	}
	return requestResult;
}

bool UdpClient::saveDate(char* buf, int s) {
	int i;
    char ch;
	string date;

    for (i = 0; i < s; i++) {
        ch = buf[i];
        if (ch == '*') { //EOF
			_today = atoi(date.c_str());
			cout << "today: " << _today << endl;
            return 1;
		}
        else {
        	date.append(to_string(ch));
		}
    }
	return 0;
}

bool UdpClient::requestHistory(int last_few_days) { //last few days
	int* days = new int[last_few_days];
	bool* days_ret = new bool[last_few_days];
	bool ret = false;

	while(!ret) {
		ret = requestToday();
	}
	getDayToUtc();

	for(int i = 0; i<last_few_days; i++) {
		days[i] = getOtherDay(i);
		days_ret[i] = false;
		cout << "day" << i << ": " << days[i] <<endl;
	}
    
    for(int i = 0; i<last_few_days; i++) {
		while(!days_ret[i])
			days_ret[i] = requestFile(to_string(days[i]));
		sleep(2);
	}

	delete [] days;
	delete [] days_ret;

    return true;
}

bool UdpClient::requestFile(string filename) 
{
	bool requestResult = false;

	if(!_data_man.openNewfile(filename)) {
		requestResult = true;
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