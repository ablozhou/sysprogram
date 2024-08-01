/*
Implement a program in which two unrelated processes, P1 and P2 communicated using two FIFOs. P2 creates a “request” FIFO whereas P1 creates a “response” FIFO. P1 writes a string of any length (up to a maximum) to the request FIFO. P2 reads the string from the request FIFO, converts it to uppercase, and sends the result to P1 using the response FIFO. P1 then prints the string received as response. Using exit handlers, each process deletes the FIFO it created. Test this program out with a variety of sample inputs.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

#define TRUE 1
#define MODE 0644
#define MAX_SIZE 1024
#define FIFO1 "/home/zhh/myfifo1"
#define FIFO2 "/home/zhh/myfifo2"

int fdr = 0;
int fdw = 0;
int fdDummy = 0;

void cleanup(void)
{
    printf("cleanup.\n");

    if (fdr > 0)
    {
        if (close(fdr) < 0)
        {
            printf("close fdr failed.\n");
        }
    }
    if (fdw > 0)
    {
        if (close(fdw) < 0)
        {
            printf("close fdw failed.\n");
        }
    }
    // if (fdDummy > 0)
    // {
    //     if (close(fdDummy) < 0)
    //     {
    //         printf("close fdDummy failed.\n");
    //     }
    // }
    // if (unlink(FIFO1) < 0)
    // {
    //     printf("unlink %s failed.\n", FIFO1);
    // }
    if (unlink(FIFO2) < 0)
    {
        printf("unlink %s failed.\n", FIFO2);
    }
}

void signal_handler(int signal)
{
    exit(0);
}

void main(int argc, char *argv[])
{
    int status;
    int nRead;
    int nWrite;
    char buffer[MAX_SIZE];

    // if (argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    // {
    //     printf("Usage: %s -h | --help | \"string to send\"\n", argv[0]);
    //     exit(1);
    // }
    // register clean up
    status = atexit(cleanup);
    // check
    if (status < 0)
    {
        printf("register clean up failed.\n");
        exit(1);
    }
    // register signal
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        printf("signal_handler register failed.\n");
        exit(1);
    }
    // the open order to avoid deadlock, because open read will wait right open.
    fdw = open(FIFO1, O_WRONLY);
    if (fdw < 0)
    {
        printf("open %s to write failed.\n", FIFO1);
        exit(1);
    }

    fdr = open(FIFO2, O_RDONLY);
    if (fdr < 0)
    {
        printf("open %s to read failed.\n", FIFO2);
        exit(1);
    }
    fdDummy = open(FIFO2, O_WRONLY);
    if (fdDummy < 0)
    {
        printf("open dummy %s to write failed.\n", FIFO2);
        exit(1);
    }

    while (TRUE)
    {
        printf("write message to send ...\n");
        char *s = fgets(buffer, MAX_SIZE - 1, stdin);
        if (s == NULL)
        {
            continue;
        }
        nRead = strlen(s);
        printf("get: %s, len:%d\n", s, nRead);
        if (s[nRead - 1] == '\n')
        {
            printf("replace \\n\n");
            s[nRead - 1] = '\0';
        }
        printf("sending: %s, len:%ld.\n", s, strlen(s));
        nWrite = write(fdw, s, strlen(s));
        if (nWrite != strlen(s))
        {
            printf("write failed.\n");
            exit(1);
        }

        nRead = read(fdr, buffer, MAX_SIZE - 1);
        if (nRead < 0)
        {
            printf("read failed.\n");
            exit(1);
        }

        buffer[nRead] = '\0';
        printf("%s\n", buffer);

        sleep(1);
    }
}