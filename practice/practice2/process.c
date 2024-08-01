/*
Suppose a parent process with id p creates three child processes with ids x, y, and z.
After creating each child process, the parent prints a message, such as:
          Parent process p created child process x
          Parent process p created child process y
          Parent process p created child process z
When child process x gets created, it prints
    Child process x going to sleep for 10 seconds
and sleeps for 10 seconds using the sleep system call. At the end of 10 seconds, it should print
    Exiting child process x
and exit. Child processes y and z should do the same thing, but the duration of sleep should be
5 seconds and 2 seconds, respectively. Write a C program for the above and observe
the output under the following conditions:
(a) Parent process does not wait for the child processes.
(b) Parent process waits for the child processes using the wait system call.
(c) Parent process waits for the child processes using the waitpid system call, with pid being set to
x, y, and z in that order.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define SIZE 3

void say_bye()
{
    printf("Exiting child process %d\n", getpid());
}
void my_fork()
{
    pid_t pid[SIZE];
    int wait_time[SIZE] = {10, 5, 2};
    pid_t ppid = getpid();

    int status = atexit(say_bye);
    if (status == -1)
    {
        printf("register atexit error\n");
        exit(1);
    }
    int wstatus;
    for (int i = 0; i < SIZE; i++)
    {
        pid[i] = fork();
        switch (pid[i])
        {
        case -1: // error
            perror("create child failed\n");
            exit(1);
        case 0: // child
            printf("Child process %d going to sleep for %d seconds\n", getpid(), wait_time[i]);
            sleep(wait_time[i]);

            exit(0);

        default: // parent
            printf("Parent process %d created child process %d\n", ppid, pid[i]);
            break;
        }
    }
    for (int i = 0; i < SIZE; i++)
    {

        pid_t p = waitpid(pid[SIZE - i - 1], &wstatus, 0);
        if (p == -1)
        {
            printf("error wait");
            exit(1);
        }
        printf("Exited child process %d\n", p);
    }
    _exit(0);
}

void main(int argc, char *argv[])
{
    my_fork();
}