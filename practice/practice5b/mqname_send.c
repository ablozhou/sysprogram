/*
3. Implement a POSIX message queue with a writer process and a reader process. The writer writes messages to the message queue, where each message represents a Person with a firstName (20 bytes) and a lastName (20 bytes). The reader reads all the messages in the message queue and displays them to screen.

    (a)  Set the message priorities such that they are read in FIFO order.

    (b)  Set the message priorities such that they are read in LIFO order.

    (c)  Set the maximum number of messages in the message queue to 10. What happens if the writer process tries to write 100 messages to the message queue? What happens if the reader is run 10 times?
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>

#define TRUE 1
#define NAME_LEN 20
#define MSG_COUNT 10
#define ARRAY_SIZE 10
#define MQ "/mymq"

typedef struct
{
    char first[NAME_LEN];
    char last[NAME_LEN];
} Names;

mqd_t mqd = (mqd_t)0;

void cleanup(void)
{
    if (mqd != (mqd_t)0)
    {
        if (mq_close(mqd) != 0)
        {
            printf("Error close mqd.\n");
        }
        else
        {
            printf("Closed mqd.\n");
        }
    }
}

void main(int argc, char *argv[])
{

    int status;
    int nRead;
    Names names[ARRAY_SIZE] = {

        {"Alice", "Johnson"},
        {"Bob", "Brown"},
        {"Charlie", "Davis"},
        {"David", "Wilson"},
        {"Eva", "Taylor"},
        {"Frank", "Anderson"},
        {"Grace", "Thomas"},
        {"Henry", "Moore"},
        {"Iane", "Smith"},
        {"John", "Doe"},

    };

    status = atexit(cleanup);
    if (status == -1)
    {
        printf("Error: registe cleanup.\n");
        exit(1);
    }
    mqd = mq_open(MQ, O_WRONLY);
    int priority;
    for (int i = 0; i < ARRAY_SIZE * 2; i++)
    {
        // priority = ARRAY_SIZE - i;
        priority = i;
        printf("Sending %d:%s %s\n", priority, names[i % ARRAY_SIZE].first, names[i % ARRAY_SIZE].last);
        status = mq_send(mqd, (const char *)&names[i % ARRAY_SIZE], sizeof(Names), priority);
        if (status < 0)
        {
            printf("Send to server failed: %s\n", strerror(errno));
            exit(1);
        }
    }
}