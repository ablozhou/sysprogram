/*
1. Create a FIFO based client-server application that does the following. The client, when started,randomly generates 10 numbers of type double, all of which are between 0 and 1. It then sends a request to the server, where the request consists of an array with the above 10 double precision floating point numbers. When the server receives a request, it sends a response to the client which consists of the average of the 10 numbers in the request. Once a client receives a response, it prints out the 10 numbers it had sent and the response it received, and then exits. The server exits on CTRL+C.

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

#include <time.h>

#define TRUE 1
#define FALSE 0
#define SERVER_FIFO "/home/zhh/fifoserver"
#define CLIENT_FIFO_TEMP "/home/zhh/fifoclient%d"

#define MAX_BUFFER_SIZE 1024
#define MSG_COUNT 10
typedef struct
{
    int id;
    double numbers[MSG_COUNT];
} Msgs;

int fdClient = 0;
char clientFifoName[MAX_BUFFER_SIZE] = {'\0'};
void cleanup(void)
{
    if (fdClient != 0)
    {
        if (close(fdClient) != 0)
        {
            printf("close fdClient failed.\n");
        }
        else
        {
            printf("close fdClient succeeded.\n");
        }
    }
    if (strlen(clientFifoName) > 0)
    {
        if (unlink(clientFifoName) == -1)
        {
            printf("Error: unlink(%s) failed.\n", clientFifoName);
        }
        else
        {
            printf("unlink(%s) succeded.\n", clientFifoName);
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
    int fdServer;

    int nRead, nWrite;

    if (argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        printf("Usage: %s clientid\n", argv[0]);
        exit(1);
    }

    Msgs msgs;
    msgs.id = atoi(argv[1]);

    status = atexit(cleanup);
    if (status < 0)
    {
        printf("Error: atexit register failed.\n");
        exit(1);
    }
    srand(time(NULL));

    fdServer = open(SERVER_FIFO, O_WRONLY);
    if (fdServer == -1)
    {
        printf("Error open fifo %s.\n", SERVER_FIFO);
        exit(1);
    }
    snprintf(clientFifoName, MAX_BUFFER_SIZE, CLIENT_FIFO_TEMP, msgs.id);
    status = mkfifo(clientFifoName, 0644);
    if (status < 0)
    {
        printf("Error: mkfifo %s failed.\n", clientFifoName);
        exit(1);
    }
    // generage randoms
    randoms(&msgs);

    nWrite = write(fdServer, &msgs, sizeof(Msgs));
    if (nWrite != sizeof(Msgs))
    {
        printf("Error writing to server fifo.\n");
        exit(1);
    }

    fdClient = open(clientFifoName, O_RDONLY);
    if (fdClient == -1)
    {
        printf("Error opening client fifo %s.\n", clientFifoName);
        exit(1);
    }
    double avg = -1.0;
    nRead = read(fdClient, &avg, sizeof(double));
    if (nRead != sizeof(double))
    {
        printf("Error reading from server fifo.\n");
        exit(1);
    }

    // printf("client id:%d\n", msgs.id);
    // for (int i = 0; i < MSG_COUNT; i++)
    // {
    //     printf("%f, ", msgs.numbers[i]);
    // }
    printf("average from server is: %f\n", avg);

    if (close(fdServer) != 0)
    {
        printf("close fdServer failed.\n");
    }
    exit(0);
}
