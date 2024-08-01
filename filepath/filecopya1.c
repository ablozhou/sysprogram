#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SIZE 1024
void copy(char *src, char *dst, int append)
{
    int idRead, idWrite;
    ssize_t nRead, nWrite;
    int buf[SIZE];
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    if (src == NULL || dst == NULL)
    {
        printf("src or dst file error\n");
        exit(1);
    }

    idRead = open(src, O_RDONLY);
    if (idRead == -1)
    {
        printf("No such file or directory: %s\n", src);
        exit(1);
    }
    if (append == 1)
    {
        idWrite = open(dst, O_WRONLY | O_CREAT | O_APPEND, mode);
    }
    else
    {
        idWrite = open(dst, O_WRONLY | O_CREAT | O_TRUNC, mode);
    }

    if (idWrite == -1)
    {
        printf("open dst file: %s failed\n", dst);
        exit(1);
    }

    while ((nRead = read(idRead, buf, SIZE)) > 0)
    {
        nWrite = write(idWrite, buf, nRead);
        if (nRead != nWrite)
        {
            printf("copy file error\n");
            exit(1);
        }
    }
    close(idRead);
    close(idWrite);
}

void main(int argc, char *argv[])
{
    int append = 0;
    if (argc < 3 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        printf("Usage: %s source destination [-a]\n", argv[0]);
        exit(1);
    }
    if (argc == 4 && strcmp(argv[3], "-a") == 0)
    {
        append = 1;
    }

    copy(argv[1], argv[2], append);
    exit(0);
}
