// my own file copy
// ./copy source.txt destination.txt -a
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 1024
void filecopy(char *src, char *dst, int append)
{
    int sid, did;
    int nread, nwrite;
    char buffer[SIZE];

    if (src == NULL || dst == NULL)
    {
        perror("src or dest file is NULL\n");
        exit(1);
    }

    sid = open(src, O_RDONLY);
    if (sid == -1)
    {
        printf("error open file %s\n", src);
        exit(1);
    }
    mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IROTH;
    if (append == 1)
    {
        did = open(dst, O_WRONLY | O_CREAT | O_APPEND, mode);
    }
    else
    {
        did = open(dst, O_WRONLY | O_CREAT | O_TRUNC, mode);
    }

    if (did == -1)
    {
        printf("error open file %s\n", dst);
        close(sid);
        exit(1);
    }

    while ((nread = read(sid, buffer, SIZE)) > 0)
    {
        nwrite = write(did, buffer, nread);
        if (nread != nwrite)
        {
            perror("error copy file\n");
            close(sid);
            close(did);
            exit(1);
        }
    }
    close(sid);
    close(did);
    exit(0);
}

void main(int argc, char *argv[])
{
    int append = 0;
    if (argc < 3 || strcmp(argv[1], "-h") == 0)
    {
        printf("Usage: %s src dst [-a]\n", argv[0]);
        exit(1);
    }
    if (argc == 4 && strcmp(argv[3], "-a") == 0)
    {
        append = 1;
    }
    filecopy(argv[1], argv[2], append);
    
}