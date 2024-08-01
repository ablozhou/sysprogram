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
#include <ctype.h>

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
    printf("cleanup\n");

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
    if (fdDummy > 0)
    {
        if (close(fdDummy) < 0)
        {
            printf("close fdDummy failed.\n");
        }
    }
    if (unlink(FIFO1) < 0)
    {
        printf("unlink %s failed.\n", FIFO1);
    }
    // if (unlink(FIFO2) < 0)
    // {
    //     printf("unlink %s failed.\n", FIFO2);
    // }
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
    //     printf("Usage: %s -h | --help\n", argv[0]);
    //     exit(1);
    // }
    // register clean up
    printf("Register cleanup.\n");
    status = atexit(cleanup);
    // check
    if (status < 0)
    {
        printf("register clean up failed.\n");
        exit(1);
    }
    // register signal
    printf("Register signal.\n");
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        printf("signal_handler register failed.\n");
        exit(1);
    }
    printf("mkfifo ...\n");
    status = mkfifo(FIFO1, MODE);
    if (status < 0)
    {
        printf("mkfifo %s failed.\n", FIFO1);
        exit(1);
    }
    status = mkfifo(FIFO2, MODE);
    if (status < 0)
    {
        printf("mkfifo %s failed.\n", FIFO2);
        exit(1);
    }
    printf("open fifo to read, blocking until write fifo open ...\n");
    fdr = open(FIFO1, O_RDONLY);
    if (fdr < 0)
    {
        printf("open %s to read failed.\n", FIFO1);
        exit(1);
    }
    fdDummy = open(FIFO1, O_WRONLY);
    if (fdDummy < 0)
    {
        printf("open dummy %s to write failed.\n", FIFO1);
        exit(1);
    }
    fdw = open(FIFO2, O_WRONLY);
    if (fdw < 0)
    {
        printf("open %s to write failed.\n", FIFO2);
        exit(1);
    }
    printf("reading ...\n");
    while (TRUE)
    {
        nRead = read(fdr, buffer, MAX_SIZE - 1);
        if (nRead < 0)
        {
            printf("read failed.\n");
            exit(1);
        }

        buffer[nRead] = '\0';
        printf("%s,length:%d\n", buffer, nRead);
        for (int i = 0; i < nRead; i++)
        {
            buffer[i] = toupper(buffer[i]);
        }
        printf("toupper:%s\n", buffer);
        nWrite = write(fdw, buffer, nRead);
        if (nWrite != nRead)
        {
            printf("write failed.\n");
            exit(1);
        }
        sleep(1);
    }
}