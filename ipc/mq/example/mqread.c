/*
 Read from a message queue and print the messages to the terminal
 Compile using:
 cc mq2.c -o mq2 -Werror -lrt
 Example of how this program should be run:
 ./mq2 /MyMessageQueue
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

int main(int argc, char* argv[])
{
    mqd_t descriptor;
    struct mq_attr attributes;
    int value;
    unsigned int numOfMessages;
    int index, status;
    int numRead;
    unsigned int priority;

    if (argc != 2 || 
        strcmp(argv[1], "-h") == 0 || 
        strcmp(argv[1], "--help") == 0)
    {
        printf("Usage %s message_queue_name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    descriptor = mq_open(
        argv[1],
        O_RDONLY
    );
    if (descriptor == -1)
    {
        printf("Failed to open message queue.\n");
        exit(EXIT_FAILURE);
    }

    status = mq_getattr(descriptor, &attributes);
    if (status == -1)
    {
        printf("Failed to read attributes.\n");
        exit(EXIT_FAILURE);
    }

    numOfMessages = attributes.mq_curmsgs;

    for (index = 0; index < numOfMessages; index++)
    {
        numRead = mq_receive(
            descriptor,
            (char*) &value,
            sizeof(int),
            &priority
        );

        if (numRead == -1)
        {
            printf("Failed to read from message queue.\n");
            exit(EXIT_FAILURE);
        }

        printf("Value = %d, priority = %u\n", value, priority);
    }


    status = mq_close(descriptor);
    if (status == -1)
    {
        printf("Failed to close message queue.\n");
        exit(EXIT_FAILURE);
    }
}