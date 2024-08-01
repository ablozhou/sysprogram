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
#include <errno.h>

#define TRUE 1
#define FALSE 0
#define SERVER_MQ "/mqserver"
#define CLIENT_MQ "/mqclient"

#define MAX_BUFFER_SIZE 1024
#define MSG_COUNT 10

// P1

void main(int argc, char *argv[])
{
    int status;
    ssize_t nRead, nWrite;

    if (argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        printf("Usage: %s \"string to send\"\n", argv[0]);
        exit(1);
    }

    char buffer[MAX_BUFFER_SIZE];

    mqd_t mqdc, mqds;
    // NOTICE: not mqd_attr_t, not mq_attr_t, not mq_attr, use: struct mq_attr
    struct mq_attr attr;

    attr.mq_maxmsg = MSG_COUNT;
    attr.mq_msgsize = MAX_BUFFER_SIZE;

    mqdc = mq_open(CLIENT_MQ, O_CREAT | O_RDONLY, 0644, &attr);
    if (mqdc == (mqd_t)-1)
    {
        printf("Error opening message queue to read.\n");
        exit(1);
    }

    if (mq_getattr(mqdc, &attr) == -1)
    {
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }

    printf("mq_maxmsg: %ld\n", attr.mq_maxmsg);
    printf("mq_msgsize: %ld\n", attr.mq_msgsize);
    printf("mq_curmsgs: %ld\n", attr.mq_curmsgs);

    mqds = mq_open(SERVER_MQ, O_WRONLY);
    if (mqds == (mqd_t)-1)
    {
        printf("Error opening message queue.\n");
        exit(1);
    }
    printf("Sending: %s\n", argv[1]);
    status = mq_send(mqds, argv[1], strlen(argv[1]), 0);
    if (status != 0)
    {
        printf("Error sending message %s.\n", argv[1]);
        exit(1);
    }

    int priority;
    // Notice: will block if mq is empty
    nRead = mq_receive(mqdc, buffer, MAX_BUFFER_SIZE, &priority);
    if (nRead < 0)
    {
        printf("Error reading message from server.\n");
        exit(1);
    }

    buffer[nRead] = '\0';
    printf("Received:%s, len:%ld\n", buffer, nRead);
    // printf("%s", buffer);

    status = mq_close(mqdc);
    if (status != 0)
    {
        printf("Error closing client mq.\n");
        exit(1);
    }
    // NOTICE not unlink
    status = mq_unlink(CLIENT_MQ);
    if (status != 0)
    {
        printf("Error unlink client mq, %s.\n", strerror(errno));
        exit(1);
    }
    status = mq_close(mqds);
    if (status != 0)
    {
        printf("Error closing server mq.\n");
        exit(1);
    }

    exit(0);
}