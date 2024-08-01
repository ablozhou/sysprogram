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
#include <signal.h>
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

int fdServer = 0, fdDummy = 0;
void cleanup(void)
{
    if (fdServer != 0)
    {
        if (close(fdServer) != 0)
        {
            printf("close fdServer failed.\n");
        }
    }
    if (fdDummy != 0)
    {
        if (close(fdDummy) != 0)
        {
            printf("close fdDummy failed.\n");
        }
    }
    if (unlink(SERVER_FIFO) == -1)
    {
        printf("Error: unlink(%s) failed.\n", SERVER_FIFO);
    }
    else
    {
        printf("unlink(%s) succeded.\n", SERVER_FIFO);
    }
}
// server
void signal_handle(int signal)
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
    int fdClient;
    char clientFifo[MAX_BUFFER_SIZE];
    int nRead, nWrite;

    Msgs msgs;

    status = atexit(cleanup);
    if (status < 0)
    {
        printf("Error: atexit register failed.\n");
        exit(1);
    }

    if (signal(SIGINT, signal_handle) == SIG_ERR)
    {
        printf("Error: signal register failed.\n");
        exit(1);
    }
    printf("server starting...\n");
    status = mkfifo(SERVER_FIFO, 0644);
    if (status < 0)
    {
        printf("Error: mkfifo %s failed.\n", SERVER_FIFO);
        exit(1);
    }

    fdServer = open(SERVER_FIFO, O_RDONLY);
    if (fdServer == -1)
    {
        printf("Error open fifo %s.\n", SERVER_FIFO);
        exit(1);
    }
    fdDummy = open(SERVER_FIFO, O_WRONLY);
    if (fdDummy == -1)
    {
        printf("Error open dummy fifo %s.\n", SERVER_FIFO);
        exit(1);
    }

    while (TRUE)
    {
        printf("waiting request....\n");
        nRead = read(fdServer, &msgs, sizeof(Msgs));
        if (nRead == -1)
        {
            printf("Error reading fifo %s.\n", SERVER_FIFO);
            exit(1);
        }
        // open client fifo
        // format client fifo
        snprintf(clientFifo, MAX_BUFFER_SIZE, CLIENT_FIFO_TEMP, msgs.id);
        fdClient = open(clientFifo, O_WRONLY);
        if (fdClient == -1)
        {
            printf("Error opening client fifo %s.\n", clientFifo);
            exit(1);
        }

        printf("received request from client %d\n", msgs.id);
        // calculate average number
        double avg = average(&msgs);
        // send back
        nWrite = write(fdClient, &avg, sizeof(double));
        if (nWrite != sizeof(double))
        {
            printf("Error writing client fifo %s.\n", clientFifo);
            exit(1);
        }
        // close fd
        status = close(fdClient);
        if (status != 0)
        {
            printf("Error closing client fifo %s.\n", clientFifo);
            exit(1);
        }
    }
    exit(0);
}
