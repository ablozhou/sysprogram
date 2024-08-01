#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <mqueue.h>

#define MQUEUE "/myqueue"
#define TRUE 1
#define MAX_MESSAGE 50

void cleanup(void)
{
    // mq_close(mqd);
    mq_unlink(MQUEUE);
}
void signal_handler(int signal)
{
    exit(0);
}
void main(int argc, char *argv[])
{
    int status;
    int nRead;
    struct mq_attr attr;

    // two for creating, 1. size for boundaries. 2. maximum messages to hold
    // one for reading

    atexit(cleanup);
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        printf("signal_handler failed.\n");
        exit(1);
    }

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
    mqd_t mqd = mq_open(MQUEUE, O_RDONLY);
    if (mqd == (mqd_t)-1)
    {
        printf("Couldn't open mq %s.\n", MQUEUE);
        exit(1);
    }
    int v = 0;
    int priority = 0;

    // get number of messages in the queue

    status = mq_getattr(mqd, &attr);
    if (status == -1)
    {
        printf("Couldn't get attribute.\n");
        exit(1);
    }
    size_t numMsgs = attr.mq_curmsgs;

    for (size_t i = 0; i < numMsgs; i++)
    {
        /*
        If the queue is empty, then, by default, mq_receive() blocks until a
       message becomes available, or the call is interrupted by a signal han‐
       dler.
        */
        nRead = mq_receive(mqd, (char *)&v, sizeof(int), &priority);
        if (nRead == -1)
        {
            printf("error sending message.\n");
            exit(1);
        }
        printf("received %d, priority: %d.\n", v, priority);
    }
    mq_close(mqd);
}