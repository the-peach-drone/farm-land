#ifndef _DATA_MANAGEMENT_H
#define _DATA_MANAGEMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include <fstream>
#include <filesystem>

#define BUF_SIZE 64
#define sendrecvflag 0

namespace fs = std::filesystem;
using namespace std;

class DataManagement
{
    public: 
        DataManagement();
        ~DataManagement();
        bool openNewfile(string file_name);

        int saveFile(char* buf, int s);
        int closeFile();

        void clearBuf(char* b);
    private:
        ofstream _writeFile;
        string _path;

};

#endif