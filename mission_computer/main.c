#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

//사이즈 큰 경우
//csv 저장 코드 구현
// 연결 코드 보완, 중복 파일 다운로드 x, 수신 끊길 경우
// daemon

//ssid:DroneBridge ESP32
//dronebridge
// ip 192.168.6.1, port 14550

#define BUF_SIZE 64

#define cipherKey 'S'
#define sendrecvflag 0

FILE* fp;
void error_handling(char *message);

void clearBuf(char* b) {
	int i;
	for (i=0; i <BUF_SIZE; i++)
		b[i] = '\0';
}

// function for decryption
char Cipher(char ch)
{
    return ch ^ cipherKey;
}

// function to receive file
int recvFile(char* buf, int s)
{
    int i;
    char ch;
    for (i = 0; i < s; i++) {
        ch = buf[i];
        // ch = Cipher(ch);
        if (ch == '*') {
			printf("\n");
            return 1;
		}
        else {
        	printf("%c", ch);
			fputc(ch, fp);   // 파일에 문자 하나씩 저장
		}
    }
    return 0;
}

int main(int argc, char *argv[])
{
	int sock;
	char message[BUF_SIZE];
	int str_len;
	socklen_t adr_sz;
	fp = fopen("asdf.csv", "w");
			
	struct sockaddr_in serv_adr, from_adr;
	if(argc != 3){
			printf("Usage: %s <IP> <PORT>\n", argv[0]);
			exit(1);
	}

	sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(sock == -1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

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
		break;
	}
	fclose(fp); //close 해야 파일 저장됨
	close(sock);
	
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
