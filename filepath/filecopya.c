#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#define SIZE 1024
//O_APPEND
//int open(const char *pathname, int flags, ...
                  /* mode_t mode */ 
void filecopy(char* src, char* dst, bool append) {
    int srcId = 0, dstId = 0;
    int nRead, nWrite;

    char buffer[SIZE];

    srcId = open(src, O_RDONLY);
    if(srcId <0 ){
        printf("Error open src file:%s",src);
        exit(1);
    }
    if (append) {
        dstId = open(dst, O_CREAT|O_WRONLY|O_APPEND, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IROTH);
        
    }else{
        dstId = open(dst, O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IROTH);
    }
    if (dstId <0) {
        printf("Error open dst file:%s",src);
        exit(1);
    }

    while( (nRead = read(srcId, buffer, SIZE ))>0) {
        nWrite = write(dstId, buffer, nRead);
        if(nWrite != nRead) {
            printf("write to dst file error!");
            exit(1);
        }
    }

    close(srcId);
    close(dstId);
}

bool checkUsage(int argc, char *argv[])
{
    if (argc <3 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        printf("Usage: %s srcFile dstFile [-a]\n \t -a is an option flag for append to the dst file. \n", argv[0]);
        exit(1);
    }

    if (argc == 4) {
        if (strcmp(argv[3],"-a")==0) {
            return true;
        }
    }
    return false;
}

void main(int argc, char *argv[]) {

    bool append = checkUsage(argc, argv);

    filecopy(argv[1], argv[2], append);
}