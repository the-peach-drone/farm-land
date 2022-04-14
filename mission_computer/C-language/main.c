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
	int str_len;
	socklen_t adr_sz;
	fp = fopen("asdf.csv", "w");
			
	struct sockaddr_in serv_adr, from_adr;
	if(argc != 3){
			printf("Usage: %s <IP> <PORT>\n", argv[0]); //IP: 192.168.6.1, PORT: 14550
			exit(1);
	}

	sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(sock == -1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	//requestFile();
	while(1)
	{
		fputs("Insert message(q to quit): ", stdout);
		fgets(message, sizeof(message), stdin);
		
		if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;

		sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));

		adr_sz = sizeof(from_adr);
		while(1)
		{
			clearBuf(message);

			recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*)&from_adr, &adr_sz);

			if (recvFile(message, BUF_SIZE)) {
				break;
			}
		}
		fclose(fp); //close 해야 파일 저장됨
	}
	close(sock);
	
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
