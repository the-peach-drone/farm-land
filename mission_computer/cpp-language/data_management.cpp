#include "data_management.h"

DataManagement::DataManagement() {
    
}

DataManagement::~DataManagement() {

}

void DataManagement::clearBuf(char* b) {
	int i;
	for (i=0; i <BUF_SIZE; i++)
		b[i] = '\0';
}

void DataManagement::openNewfile(const char* file_name) {
    fp = fopen(file_name, "w");
}

// function to receive file
int DataManagement::saveFile(char* buf, int s)
{
    int i;
    char ch;
    for (i = 0; i < s; i++) {
        ch = buf[i];
        if (ch == '*') { //EOF
            fclose(fp);
            return 1;
		}
        else {
        	printf("%c", ch);
			fputc(ch, fp);   // 파일에 문자 하나씩 저장
		}
    }
    return 0;
}