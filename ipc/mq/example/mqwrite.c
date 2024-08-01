/*
 Create a message queue and write 1, 3, 5, 7 to it
 Compile using:
 cc mq1.c -o mq1 -Werror -lrt
 Example of how this program should be run:
 ./mq1 /MyMessageQueue
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

int main(int argc, char *argv[])
{
    mqd_t descriptor;
    struct mq_attr attributes;
    int messages[] = {1, 3, 5, 7};
    unsigned int priority = 0;
    int status;
    int index;

    if (argc != 2 ||
        strcmp(argv[1], "-h") == 0 ||
        strcmp(argv[1], "--help") == 0)
    {
        printf("Usage %s message_queue_name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    attributes.mq_msgsize = sizeof(int);
    attributes.mq_maxmsg = 5;

    descriptor = mq_open(
        argv[1],
        O_CREAT | O_WRONLY,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,
        &attributes);

    if (descriptor == (mqd_t)-1)
    {
        printf("Failed to create message queue.\n");
        exit(EXIT_FAILURE);
    }

    for (index = 0; index < 4; index++)
    {
        priority = 0;

        status = mq_send(
            descriptor,
            (char *)&messages[index],
            sizeof(int),
            priority);

        if (status == -1)
        {
            printf("Failed to write to message queue.\n");
            exit(EXIT_FAILURE);
        }
    }

    status = mq_close(descriptor);
    if (status == -1)
    {
        printf("Failed to close message queue.\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}