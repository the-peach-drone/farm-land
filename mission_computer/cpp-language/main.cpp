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

	if(argc != 3){
			printf("Usage: %s <IP> <PORT>\n", argv[0]); //IP: 192.168.6.1, PORT: 14550
			exit(1);
	}
	
	UdpClient udp_client(argv[1], argv[2]);
	sock = udp_client.getSocket();			

	if(sock == -1)
		error_handling((char*)"socket() error");

	
	// //fputs("Insert message(q to quit): ", stdout);
	// //fgets(message, sizeof(message), stdin);
	// if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
	// 	exit(0);
		
	string filename = "20220414";
	
	udp_client.requestFile(filename);
	
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
