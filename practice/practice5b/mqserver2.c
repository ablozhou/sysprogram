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
#include <time.h>

#define TRUE 1
#define FALSE 0
#define SERVER_MQ "/mqserver"
#define CLIENT_MQ_TEMPLATE "/mqclient%d"
// #define CLIENT_MQ "/mqclient"
#define MAX_BUFFER_SIZE 1024
#define MSG_COUNT 10

typedef struct
{
    int id;
    double numbers[MSG_COUNT];
} Msgs;

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
        }
        else
        {
            printf("Server mq closed successfully.\n");
        }
    }

    // NOTICE not unlink
    status = mq_unlink(SERVER_MQ);
    if (status != 0)
    {
        printf("Error unlink server mq, %s.\n", strerror(errno));
    }
    else
    {
        printf("Server mq unlinked.\n");
    }
}

void signal_handler(int signal)
{
    exit(0);
}

double average(Msgs *msgs)
{
    double avg = 0.0;
    printf("client id: %d\n", msgs->id);
    for (int i = 0; i < MSG_COUNT; i++)
    {
        avg += msgs->numbers[i];
        printf("%f, ", msgs->numbers[i]);
    }
    avg /= MSG_COUNT;
    printf("average: %f\n", avg);
    return avg;
}
void main(int argc, char *argv[])
{
    int status;
    ssize_t nRead, nWrite;
    Msgs msgs;

    char clientMq[MAX_BUFFER_SIZE];

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
    attr.mq_msgsize = sizeof(Msgs);
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

        nRead = mq_receive(mqds, (char *)&msgs, sizeof(Msgs), &priority);
        if (nRead < 0)
        {
            printf("Error reading message from client: %s\n", strerror(errno));
            exit(1);
        }
        printf("received request from client %d\n", msgs.id);
        snprintf(clientMq, MAX_BUFFER_SIZE, CLIENT_MQ_TEMPLATE, msgs.id);

        mqdc = mq_open(clientMq, O_WRONLY);
        if (mqdc == (mqd_t)-1)
        {
            printf("Error opening message queue to read.\n");
            exit(1);
        }

        // calculate average number
        double avg = average(&msgs);
        // NOTICE: return 0 success, -1 failure, not nRead.
        status = mq_send(mqdc, (const char *)&avg, sizeof(double), 0);
        if (status == -1)
        {
            printf("Error sending message %s\n", strerror(errno));
            exit(1);
        }
        status = mq_close(mqdc);
        if (status != 0)
        {
            printf("Error closing client mq.\n");
            exit(1);
        }
    }
}