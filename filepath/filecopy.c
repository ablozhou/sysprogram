#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 1024
void filecopy(char* src, char* dst) {
    FILE* fsrc = fopen(src, "r");
    char buffer[SIZE];
    int ret;

    if(fsrc == NULL ) {
        printf("error open file.");
        exit(1);
    }
    FILE* fdst = fopen(dst, "w");
    if( fdst == NULL) {
        printf("error open file.");
        exit(1);
    }
    while((ret = fread(buffer, 1, SIZE, fsrc))>0) {
        ret = fwrite(buffer, 1, ret, fdst);
        if (ret < 0) {
            printf("write error");
            break;
        }
    }

    if(ret < 0) {
        printf("error copy file");
        
    }

    fclose(fsrc);
    fclose(fdst);
}

void filecopy1(char* src, char* dst) {
    int srcId, dstId;
    int numRead, numWrite;
    int status;
    char buffer[SIZE];
    srcId = open(src, O_RDONLY);

    if(srcId == -1 ) {
        printf("error open file.");
        exit(1);
    }
    dstId = open(dst, O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IROTH);
    if( dstId == -1) {
        printf("error open file.");
        exit(1);
    }

    numRead = read(srcId, buffer, SIZE );
    while(numRead>0) {
        numWrite = write(dstId, buffer, numRead);
        if (numRead!= numWrite) {
            printf("error write file");
            exit(1);
        }
        numRead = read(srcId, buffer, SIZE );
    }

    status = close(srcId);
    if (status == -1) {
        printf("Failed to close source file");
        exit(1);
    }
    status = close(dstId);
    if (status == -1) {
        printf("Failed to close target file");
        exit(1);
    }
}
void main(int argc, char* argv[]) {

    if (argc !=3 || strcmp(argv[1], "-h")==0 || strcmp(argv[1], "--help") == 0 ) {
        printf("Usage: %s src, dst\n", argv[0]);
        exit(1);
    }

    filecopy1(argv[1], argv[2]);

}