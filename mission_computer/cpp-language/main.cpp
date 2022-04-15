#include "main.h"

//사이즈 큰 경우
//csv 저장 코드 구현
// 연결 코드 보완, 중복 파일 다운로드 x, 수신 끊길 경우
// daemon

//ssid:DroneBridge ESP32
//pw: dronebridge

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock;
	char message[BUF_SIZE];
	
	UdpClient udp_client("192.168.6.1", 14550);
	sock = udp_client.getSocket();			

	if(sock == -1)
		error_handling((char*)"socket() error");

	string filename = "20220414";
	
	int ret = 0;
	while (!ret)
	{
		ret = udp_client.requestFile(filename);
	}
	// udp_client.requestFile("20220412");
	// udp_client.requestFile("20220413");
	
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
