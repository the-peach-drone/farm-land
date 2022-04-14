#ifndef _DATA_MANAGEMENT_H
#define _DATA_MANAGEMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <unistd.h>

#define BUF_SIZE 64
#define sendrecvflag 0

using namespace std;

class DataManagement
{
    public: 
        DataManagement();
        ~DataManagement();
        void openNewfile(const char* file_name);
        int saveFile(char* buf, int s);
        void clearBuf(char* b);
    private:
        FILE* fp;

};

#endif