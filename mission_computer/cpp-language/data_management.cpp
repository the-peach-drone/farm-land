#include "data_management.h"

DataManagement::DataManagement() {
    uint16_t folder_name = 0;
    std::string dir = "/home/ubuntu/";
    dir.append("/files/0");

    while(!fs::create_directories(dir))
    {
        dir = "/home/ubuntu/";
        dir.append("/files/");
        folder_name++;
        dir.append(to_string(folder_name));
        dir.append("/");
    } 
    cout << _path <<endl;
    _path = dir;
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
    string path = _path;
    file_name.append(".csv");
    path.append(file_name);

    if(_writeFile.is_open()) {
        _writeFile.close();
    }
   
    if(access(path.c_str(), F_OK) == 0){
        std::cout << "already exist file" << std::endl;
        return 0;
    }else {
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