/*
Write the integers 1,3,5,7 to message queue
Compile:
    cc mqwrite.c -o mqwrite -Werror -lrt
Run:
    ./mqwrite
    seq 1 10 | xargs -n 1 ./write
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define MQUEUE "/myqueue"
#define TRUE 1
#define MAX_MESSAGE 10

void main(int argc, char *argv[])
{
    int status;
    struct mq_attr attr;
    // two for creating, 1. size for boundaries. 2. maximum messages to hold
    // one for reading
    if (argc < 2)
    {
        printf("Usage: %s value\n", argv[0]);
        exit(1);
    }

    attr.mq_msgsize = sizeof(int);
    attr.mq_maxmsg = MAX_MESSAGE;

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
    mqd_t mqd = mq_open(MQUEUE, O_CREAT | O_WRONLY, mode, &attr);
    if (mqd == (mqd_t)-1)
    {
        printf("Couldn't open mq %s.\n", MQUEUE);
        exit(1);
    }
    int v = atoi(argv[1]);
    printf("sending %d to mq %s\n", v, MQUEUE);

    status = mq_send(mqd, (char *)&v, sizeof(int), v);
    if (status == -1)
    {
        printf("error sending message.\n");
        exit(1);
    }
    mq_close(mqd);
    // mq_unlink(MQUEUE);
}