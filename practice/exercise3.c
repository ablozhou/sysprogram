
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_SIZE 100
void main(int argc, char *argv[])
{
    int status;
    pid_t pid[2];
    int fd[2];
    int nRead, nWrite;
    char buffer[MAX_SIZE];

    if (argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        printf("Usage: %s message", argv[0]);
        exit(1);
    }

    int len = strlen(argv[1]) + 1;
    char *msg = malloc(len);
    strcpy(msg, argv[1]);
    msg[len - 1] = '\0';
    status = pipe(fd);
    if (status == -1)
    {
        printf("Error create pipe.\n");
        exit(1);
    }

    for (int i = 0; i < 2; i++)
    {
        pid[i] = fork();
        if (pid[i] == -1)
        {
            printf("fork child %d failed.\n", i);
            exit(1);
        }
        if (pid[i] == 0 && i == 0)
        { // write child
            // close read fd
            status = close(fd[0]);
            if (status == -1)
            {
                printf("Error close fd[0].\n");
                exit(1);
                // Sourav Mukherjee 2 days ago
                // Child processes should call _exit
            }

            nWrite = write(fd[1], msg, len);
            if (nWrite != len)
            {
                printf("Error write msg: %s to fd[1].\n", msg);
                exit(1);
            }
            status = close(fd[1]);
            if (status == -1)
            {
                printf("Error close fd[1].\n");
                exit(1);
            }
            exit(0);
        }
        if (pid[i] == 0 && i == 1)
        { // read child
            // close write fd
            status = close(fd[1]);
            if (status == -1)
            {
                printf("Error close fd[0].\n");
                exit(1);
            }

            nRead = read(fd[0], buffer, len);
            if (nRead == -1)
            {
                printf("Error write msg: %s to fd[1].\n", msg);
                exit(1);
            }

            for (int j = 0; j < len; j++)
            {
                buffer[j] = toupper(buffer[j]);
            }

            printf("%s\n", buffer);

            status = close(fd[0]);
            if (status == -1)
            {
                printf("Error close fd[1].\n");
                exit(1);
            }
            exit(0);
        }
    }

    for (int i = 0; i < 2; i++)
    {
        status = close(fd[i]);
        if (status == -1)
        {
            printf("Error close fd[%d].\n", i);
            exit(1);
        }
    }
    // Sourav Mukherjee 2 days ago
    // Parent process must wait for child processes to complete.

    exit(0);
}
