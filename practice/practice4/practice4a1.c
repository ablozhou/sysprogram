/*
1. Implement a program in which a parent process writes a string of any length (up to a maximum) to a pipe, and a child process reads that string from the pipe and prints out the length of the string. Test this program out with the following strings:

(a) Hello World! The length should be 12.
(b) Linux Programming The length should be 17.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX_BUFFER 1024

void main(int argc, char *argv[])
{
    int status;
    int nRead, nWrite;
    int pipefd[2];
    pid_t pid;
    char buffer[MAX_BUFFER];

    if (argc < 2)
    {
        printf("Usage: %s <\"string to test\">\n", argv[0]);
        exit(1);
    }
    char *sending = argv[1];
    status = pipe(pipefd);
    pid = fork();
    switch (pid)
    {
    case -1:
        printf("Error fork()\n");
        exit(1);
    case 0: // child
        // close write pipe
        status = close(pipefd[1]);
        nRead = read(pipefd[0], buffer, MAX_BUFFER - 1);

        buffer[nRead] = '\0';
        printf("Child received %d bytes.\n", nRead);
        printf("%s The length should be %ld\n", buffer, strlen(buffer));

        if (nRead == -1)
        {
            printf("Error reading.\n");
            exit(1);
        }
        status = close(pipefd[0]);
        exit(0);
    default: // parent
        // close read pipe
        status = close(pipefd[0]);
        sprintf(buffer, "%s", sending);

        nWrite = write(pipefd[1], buffer, strlen(buffer));

        printf("Parent send `%s` The length is %ld\n", buffer, strlen(buffer));

        if (nWrite != strlen(sending))
        {
            printf("Error writing.\n");
            exit(1);
        }
    }
    status = close(pipefd[1]);
    status = wait(&pid);
    exit(0);
}