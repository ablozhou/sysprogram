/*
1. Implement a program in which two unrelated processes, P1 and P2 communicated using POSIX message queues. P1 writes a string of any length (up to a maximum) to a “request” message queue. P2 reads the string from the message queue, converts it to uppercase, and sends the result to P1 using a “response” message queue. Test this program out with a variety of sample inputs.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <mqueue.h>
#include <ctype.h>
#include <errno.h>

#define TRUE 1
#define FALSE 0
#define SERVER_MQ "/mqserver"
#define CLIENT_MQ "/mqclient"

#define MAX_BUFFER_SIZE 1024
#define MSG_COUNT 3

mqd_t mqds = (mqd_t)0;
// P2 server
void cleanup(void)
{
    int status;
    if (mqds != (mqd_t)0)
    {
        status = mq_close(mqds);
        if (status != 0)
        {
            printf("Error closing server mq.\n");
            exit(1);
        }
    }

    // NOTICE not unlink
    status = mq_unlink(SERVER_MQ);
    if (status != 0)
    {
        printf("Error unlink server mq, %s.\n", strerror(errno));
        exit(1);
    }
}

void signal_handler(int signal)
{
    exit(0);
}
void main(int argc, char *argv[])
{
    int status;
    ssize_t nRead, nWrite;

    // if (argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    // {
    //     printf("Usage: %s \"string to send\"\n", argv[0]);
    //     exit(1);
    // }

    char buffer[MAX_BUFFER_SIZE];

    mqd_t mqdc;
    struct mq_attr attr;

    status = atexit(cleanup);
    if (status != 0)
    {
        printf("error register cleanup.\n");
        exit(1);
    }

    // for compatible
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) != 0)
    {
        printf("error register sigaction.\n");
        exit(1);
    }

    attr.mq_flags = 0;
    attr.mq_maxmsg = MSG_COUNT;
    attr.mq_msgsize = MAX_BUFFER_SIZE;
    attr.mq_curmsgs = 0;

    mqds = mq_open(SERVER_MQ, O_CREAT | O_RDWR, 0644, &attr);

    if (mqds == (mqd_t)-1)
    {
        printf("Error opening message queue.\n");
        exit(1);
    }

    printf("Waiting requests...\n");
    while (TRUE)
    {
        int priority;

        nRead = mq_receive(mqds, buffer, MAX_BUFFER_SIZE, &priority);
        if (nRead < 0)
        {
            printf("Error reading message from client: %s\n", strerror(errno));
            exit(1);
        }

        buffer[nRead] = '\0';
        printf("Received: %s\n", buffer);

        for (int i = 0; i < nRead; i++)
        {
            buffer[i] = toupper(buffer[i]);
        }

        mqdc = mq_open(CLIENT_MQ, O_WRONLY);
        if (mqdc == (mqd_t)-1)
        {
            printf("Error opening message queue to read.\n");
            exit(1);
        }

        printf("sending %s, len:%ld\n", buffer, nRead);
        // NOTICE: return 0 success, -1 failure, not nRead.
        status = mq_send(mqdc, buffer, nRead, 0);
        if (status == -1)
        {
            printf("Error sending message %s, %s\n", buffer, strerror(errno));
            exit(1);
        }
        status = mq_close(mqdc);
        if (status != 0)
        {
            printf("Error closing client mq.\n");
            exit(1);
        }
    }

    exit(0);
}