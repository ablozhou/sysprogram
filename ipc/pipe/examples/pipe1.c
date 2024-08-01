/*
 * Parent process writes 100, 150, 200, 250 to a pipe in that order
 * Child process reads them from the pipe and prints them in that order
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int pipeDescriptor[2];
    int status, childStatus;
    int values[] = {100, 150, 200, 250};
    int value;
    int childId;
    int index;
    int numWritten, numRead, numValues;

    status = pipe(pipeDescriptor);
    if (status == -1)
    {
        printf("Failed to create pipe.\n");
        exit(EXIT_FAILURE);
    }

    /* At this point, pipeDescriptor[0] is the read descriptor */
    /* pipeDescriptor[1] is the write descriptor */

    childId = fork();
    switch (childId)
    {
    case -1:
        printf("Failed to create child process.\n");
        exit(EXIT_FAILURE);

    case 0:
        /* Close the write descriptor */
        status = close(pipeDescriptor[1]);
        if (status == -1)
        {
            printf("Child failed to close write descriptor.\n");
            _exit(EXIT_FAILURE);
        }

        /* Read from the pipe until there is nothing left to read */
        numRead = read(pipeDescriptor[0], &value, sizeof(int));
        while (numRead > 0)
        {
            printf("Child read %d\n", value);
            numRead = read(pipeDescriptor[0], &value, sizeof(int));
        }

        if (numRead == -1)
        {
            printf("Child failed to read from the pipe.\n");
            _exit(EXIT_FAILURE);
        }

        /* Close the read descriptor */
        status = close(pipeDescriptor[0]);
        if (status == -1)
        {
            printf("Child failed to close read descriptor.\n");
            _exit(EXIT_FAILURE);
        }

        _exit(EXIT_SUCCESS);

    default:
        /* Close the read descriptor */
        status = close(pipeDescriptor[0]);
        if (status == -1)
        {
            printf("Parent failed to close read descriptor.\n");
            exit(EXIT_FAILURE);
        }

        /* write the values to the pipe */
        numWritten = write(pipeDescriptor[1], values, sizeof(values));
        if (numWritten != sizeof(values))
        {
            printf("Partial write error.\n");
            exit(EXIT_FAILURE);
        }

        /* Close the write descriptor */
        status = close(pipeDescriptor[1]);
        if (status == -1)
        {
            printf("Parent failed to close write descriptor.\n");
            exit(EXIT_FAILURE);
        }

        pid_t pid = wait(&childStatus);
        if (pid == -1)
        {
            printf("Failed to wait for child process to complete.\n");
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
    }
}