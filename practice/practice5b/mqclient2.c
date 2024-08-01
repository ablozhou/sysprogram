/*
 2. Create a Message Queue based client-server application that does the following. The client, when started,randomly generates 10 numbers of type double, all of which are between 0 and 1. It then sends a request to the server, where the request consists of an array with the above 10 double precision floating point numbers. When the server receives a request, it sends a response to the client which consists of the average of the 10 numbers in the request. Once a client receives a response, it prints out the 10 numbers it had sent and the response it received, and then exits. The server exits on CTRL+C.

The client and server should be run as follows.
In shell 1:

```bash
./server
```

In shell 2:

```bash
seq 1 4 | xargs -P 0 -n 1 ./client
```

The last command above runs four instances of the client, with as many of them running in parallel as possible.

(a) Implement the above client-server application where the server is a single-threaded process.

(b) Now modify the server so that it is a multi-threaded process without making any changes to the client. The main thread of the server should receive requests. Whenever a request arrives, this thread should create a child thread whose task is to respond to the request.
 */

// client
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
#define CLIENT_MQ_TEMPLATE "/mqclient%d"

#define MAX_BUFFER_SIZE 1024
#define MSG_COUNT 10

// P1
typedef struct
{
    int id;
    double numbers[MSG_COUNT];
} Msgs;

mqd_t mqdc = (mqd_t)0;
mqd_t mqds = (mqd_t)0;
char clientMq[MAX_BUFFER_SIZE] = {'\0'};

void cleanup(void)
{
    if (mqdc != (mqd_t)0)
    {
        if (mq_close(mqdc) != 0)
        {
            printf("close mqdc failed.\n");
        }
        else
        {
            printf("close mqdc succeeded.\n");
        }
    }
    if (strlen(clientMq) > 0)
    {
        if (mq_unlink(clientMq) == -1)
        {
            printf("Error: unlink(%s) failed.\n", clientMq);
        }
        else
        {
            printf("unlink(%s) succeded.\n", clientMq);
        }
    }
    if (mqds != (mqd_t)0)
    {
        if (mq_close(mqds) != 0)
        {
            printf("Error closing server mq.\n");
        }
        else
        {
            printf("Server mq closed.\n");
        }
    }
}

Msgs *randoms(Msgs *msgs)
{
    printf("client id:%d generate:\n", msgs->id);
    for (int i = 0; i < MSG_COUNT; i++)
    {
        msgs->numbers[i] = (double)rand() / RAND_MAX;
        printf("%f, ", msgs->numbers[i]);
    }
    printf("\n");
    return msgs;
}

void main(int argc, char *argv[])
{
    int status;
    ssize_t nRead, nWrite;
    Msgs msgs;

    if (argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        printf("Usage: %s clientid\n", argv[0]);
        exit(1);
    }

    atexit(cleanup);
    // char buffer[MAX_BUFFER_SIZE];
    msgs.id = atoi(argv[1]);

    srand(time(NULL));
    // NOTICE: not mqd_attr_t, not mq_attr_t, not mq_attr, use: struct mq_attr
    struct mq_attr attr;

    attr.mq_maxmsg = MSG_COUNT;
    attr.mq_msgsize = sizeof(double);

    snprintf(clientMq, MAX_BUFFER_SIZE, CLIENT_MQ_TEMPLATE, msgs.id);
    mqdc = mq_open(clientMq, O_CREAT | O_RDONLY, 0644, &attr);
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
        printf("Error opening server message queue: %s\n", strerror(errno));
        exit(1);
    }
    // generage randoms
    randoms(&msgs);
    status = mq_send(mqds, (const char *)&msgs, sizeof(Msgs), 0);
    if (status != 0)
    {
        printf("Error sending message to server: %s\n", strerror(errno));
        exit(1);
    }

    int priority;
    double value;
    nRead = mq_receive(mqdc, (char *)&value, sizeof(double), &priority);
    if (nRead < 0)
    {
        printf("Error reading message from server.\n");
        exit(1);
    }

    printf("Received avg:%lf, len:%ld\n", value, nRead);
    // printf("%s", buffer);

    // status = mq_close(mqdc);
    // if (status != 0)
    // {
    //     printf("Error closing client mq.\n");
    //     exit(1);
    // }
    // // NOTICE not unlink
    // status = mq_unlink(clientMq);
    // if (status != 0)
    // {
    //     printf("Error unlink client mq, %s.\n", strerror(errno));
    //     exit(1);
    // }

    exit(0);
}