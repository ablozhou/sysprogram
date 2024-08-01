/*
ls -al
cc --version
cal 2024
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
void say_bye()
{
    printf("Exiting child process %d\n", getpid());
}
void my_fork()
{
    pid_t pid;

    int status = atexit(say_bye);
    if (status == -1)
    {
        printf("register atexit error\n");
        exit(1);
    }
    int wstatus;

    pid = fork();
    switch (pid)
    {
    case -1: // error
        perror("create child failed\n");
        exit(1);
    case 0: // child
        printf("Child process %d created\n", getpid());
        // status = execl("/usr/bin/ls", "ls", "-ls", NULL);
        // status = execl("/usr/bin/cc", "cc", "--version", NULL);
        status = execl("/usr/bin/cal", "cal", "2024", NULL);
        if (status == -1)
        {
            printf("register atexit error\n");
            exit(1);
        }
        exit(0);

    default: // parent
        printf("Parent process %d created child process %d\n", getpid(), pid);
        break;
    }
    pid_t p = waitpid(pid, &wstatus, 0);
    if (p == -1)
    {
        printf("error wait");
        exit(1);
    }
    printf("Exited child process %d\n", p);
    _exit(0);
}

void main(int argc, char *argv[])
{
    my_fork();
}