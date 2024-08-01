// mycat
//  ./cat file1.txt file2.txt file3.txt file4.txt
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 1024

void mycat(char *files[], int size)
{
    int fid;
    int nread;
    char buffer[SIZE];

    for (int i = 0; i < size; i++)
    {
        fid = open(files[i], O_RDONLY);
        if (fid == -1)
        {
            printf("open file %s faild\n", files[i]);
            exit(1);
        }

        while ((nread = read(fid, buffer, SIZE - 1)) > 0)
        {
            buffer[nread] = '\0';
            printf("%s", buffer);
        }
        close(fid);
    }
}

void main(int argc, char *argv[])
{
    int append = 0;
    if (argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        printf("Usage: %s file ...\n", argv[0]);
        exit(1);
    }

    mycat(&argv[1], argc - 1);
    exit(0);
}
