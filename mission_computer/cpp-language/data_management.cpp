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

bool DataManagement::openNewfile(string file_name)
{
    string path = "/home/madman/";
    file_name.append(".csv");
    path.append(file_name);

    if(_writeFile.is_open())
        return 0;
   
    if(access(path.c_str(),F_OK) == 0){
        std::cout << "파일이 존재 합니다." << std::endl;
        return 0;
    }else{
        std::cout << "파일이 존재하지 않습니다." << std::endl;
        _writeFile.open(path);
        return 1;
    }
 }

// function to receive file
int DataManagement::saveFile(char* buf, int s)
{
    if(!_writeFile.is_open())
        return 0;

    int i;
    char ch;
    for (i = 0; i < s; i++) {
        ch = buf[i];
        if (ch == '*') { //EOF
            _writeFile.close();
            return 1;
		}
        else {
        	printf("%c", ch);
			_writeFile.write(&ch, 1);   // 파일에 문자 하나씩 저장
		}
    }
    return 0;
}

int DataManagement::closeFile() 
{
    if(_writeFile.is_open()) {
        _writeFile.close();
        return 1;
    }
    else    
        return 0;
}