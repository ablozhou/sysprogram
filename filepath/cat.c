#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#define SIZE 1024

void cat(char *files[], int len)
{
    int srcId = 0;
    int nRead;

    char buffer[SIZE];

    for (int i = 0; i < len; i++)
    {
        char *f = files[i];
        srcId = open(f, O_RDONLY);
        if (srcId < 0)
        {
            printf("Error open file:%s", f);
            exit(1);
        }
        // reserve a byte to set the end of string to '\0' for print or else it may encounter error code.
        while ((nRead = read(srcId, buffer, SIZE - 1)) > 0)
        {
            buffer[nRead] = '\0';
            printf("%s", buffer);
            // memset(buffer, '\0', SIZE);
        }

        close(srcId);
    }
}

void printUsage(int argc, char *argv[])
{
    if (argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        printf("Usage: %s file1, file2, ... \n \t cat all files to screen. \n", argv[0]);
        exit(1);
    }
}

void main(int argc, char *argv[])
{

    printUsage(argc, argv);

    cat(&argv[1], argc - 1);
}