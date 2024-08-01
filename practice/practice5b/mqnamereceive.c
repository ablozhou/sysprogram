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
#include <signal.h>

#define TRUE 1
#define NAME_LEN 20
#define MSG_COUNT 10
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
    // if (mq_unlink(MQ) != 0)
    // {
    //     printf("Error unlink %s.\n", MQ);
    // }
    // else
    // {
    //     printf("Unlinked %s.\n", MQ);
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

    Names names;

    status = atexit(cleanup);
    if (status == -1)
    {
        printf("Error: registe cleanup.\n");
        exit(1);
    }

    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        printf("Error signal registered.\n");
        exit(1);
    }

    // open mq to receive
    struct mq_attr attr;
    attr.mq_maxmsg = MSG_COUNT;
    attr.mq_msgsize = sizeof(Names);
    attr.mq_flags = 0;
    attr.mq_curmsgs = 0;
    // receive mq with priority
    mqd = mq_open(MQ, O_CREAT | O_RDONLY, 0644, &attr);
    if (mqd == (mqd_t)-1)
    {
        printf("Error opening mq: %s\n", strerror(errno));
        exit(1);
    }
    printf("Waiting request...\n");
    while (TRUE)
    {

        // mq_getattr(mqd, &attr);
        // printf("messages count : %d", attr.mq_curmsgs);
        int priority;
        // printf("Waiting request...\n");
        // for (int i = 0; i < attr.mq_curmsgs; i++)
        // {
        // NOTICE: force type convertion
        nRead = mq_receive(mqd, (char *)&names, sizeof(Names), &priority);
        if (nRead < 0)
        {
            printf("Error recieving mq: %s\n", strerror(errno));
            exit(1);
        }
        // print mq names
        printf("%d: %s %s\n", priority, names.first, names.last);
        //}
    }

    // close mq
    // unlink
    exit(0);
}