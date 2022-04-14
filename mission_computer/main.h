#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 64
#define sendrecvflag 0

FILE* fp;

void clearBuf(char* b) {
	int i;
	for (i=0; i <BUF_SIZE; i++)
		b[i] = '\0';
}

// function to receive file
int recvFile(char* buf, int s)
{
    int i;
    char ch;
    for (i = 0; i < s; i++) {
        ch = buf[i];
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