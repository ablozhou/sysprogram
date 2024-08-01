/*
 * Goals:
 * Parent process creates a child process.
 * Child process goes to sleep for 2 seconds and then exits.
 * Parent process waits for the child process to complete and then exits.
 * The exit system call is _exit()
 * The library function exit() is for running exit handler (if registered) after which it calls _exit()
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
    pid_t childPid;
    int childStatus, status;

    /* Register the exit handler */
    status = atexit(printBye);
    if (status != 0)
    {
        printf("Failed to register exit handler.\n");
        exit(EXIT_FAILURE);
    }

    childPid = fork();

    switch (childPid)
    {
    case -1: /* Child creation failed */
        printf("Failed to create child process.\n");
        exit(EXIT_FAILURE);

    case 0: /* Child process */
        printf(
            "I am the child process with PID %d. My parent PID is %d.\n",
            getpid(),
            getppid());

        sleep(2);
        _exit(EXIT_SUCCESS);
        //_exit(1);

    default: /* Parent process */
        printf(
            "I am the parent process with PID %d. I created a child process with PID %d.\n",
            getpid(),
            childPid);

        childPid = wait(&childStatus);
        if (childPid == -1)
        {
            printf("Failed to wait for child process.\n");
            exit(EXIT_FAILURE);
        }

        printf(
            "Child process with PID %d exited with status %d.\n",
            childPid,
            childStatus);

        exit(EXIT_SUCCESS);
    }
}