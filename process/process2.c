/*
 * Goals:
 * Parent process creates two child processes.
 * Each child processes runs "date --utc", and then exits.
 * Parent process waits for the child processes to complete and then exits.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void printBye()
{
    printf("Process %d says Bye!\n", getpid());
}

int main()
{
    pid_t parentPid, child;
    pid_t childPid[2];
    int childStatus, status, index;

    /* Register the exit handler */
    status = atexit(printBye);
    if (status != 0)
    {
        printf("Failed to register exit handler.\n");
        exit(EXIT_FAILURE);
    }

    for (index = 0; index < 2; index++)
    {
        childPid[index] = fork();

        switch (childPid[index])
        {
        case -1: /* Child creation failed */
            printf("Failed to create child process.\n");
            exit(EXIT_FAILURE);

        case 0: /* Child process */
            printf(
                "I am the child process with PID %d. My parent PID is %d.\n",
                getpid(),
                getppid());

            // status = execlp("/usr/bin/date", "date", "--utc", NULL);
            status = execlp("/usr/bin/ls", "ls", "-al", NULL);
            /* Control in the child process should not reach the next line unless exec fails */
            if (status == -1)
            {
                printf("execlp failed\n");
                _exit(1);
            }
            // code should never reach here
            printf("Failed to exec.\n");
            _exit(EXIT_FAILURE);

        default: /* Parent process */
            break;
        }
    }

    for (index = 0; index < 2; index++)
    {
        child = waitpid(childPid[index], &childStatus, 0);
        if (child == -1)
        {
            printf("Failed to wait for child process.\n");
            exit(EXIT_FAILURE);
        }

        printf(
            "Child process with PID %d exited with status %d.\n",
            child,
            childStatus);
    }

    exit(EXIT_SUCCESS);
}
