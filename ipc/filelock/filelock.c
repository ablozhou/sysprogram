/*
only one instance
Open/create the file named in 'pidFile', lock it, optionally set the
close-on-exec flag for the file descriptor, write our PID into the file,
and (in case the caller is interested) return the file descriptor
referring to the locked file. The caller is responsible for deleting 'pidFile' file (just) before process termination. 'progName' should be the name of the calling program (i.e., argv[0] or similar), and is used only for diagnostic messages. If we can't open 'pidFile', or we encounter some other error, then we print an appropriate diagnostic and terminate.
The flags argument can specify the constant CPF_CLOEXEC, which causes createPidFile() to set the close-on-exec flag for the file descriptor. This is useful for servers that restart themselves by calling exec(). If the file descriptor was not closed during the exec(), then the restarted server would think that a duplicate instance of the server is already running.
flags = fcntl(fd, F_GETFD);
flags |= FD_CLOEXEC;
if (fcntl(fd, F_SETFD, flags) == -1)

https://man7.org/tlpi/code/online/book/filelock/create_pid_file.c.html
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/file.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>

#define CPF_CLOEXEC 1
#define BUF_SIZE 100

void main(int argc, char *argv[])
{
    int status;
    if (argc < 3)
    {
        printf("Usage: %s file_name sleep_time(s)\n", argv[0]);
        exit(1);
    }
    char *file_name = argv[1];
    int sleep_time = atoi(argv[2]);

    int fd = open(argv[1], O_RDWR | O_CREAT, 0644);
    if (fd < 0)
    {
        printf("open failed.\n");
        exit(1);
    }

    // will block wait until the file is unlocked.
    // LOCK_EX LOCK_UN LOCK_SH
    status = flock(fd, LOCK_EX);
    if (status < 0)
    {
        if (errno == EWOULDBLOCK)
        {
            printf("PID %ld: already locked - bye!", (long)getpid());
        }
        else
        {
            printf("flock (PID=%ld)", (long)getpid());
        }
        exit(1);
    }
    printf("PID %ld: granted %s at %ld\n", (long)getpid(), file_name, time(NULL));
    sleep(sleep_time);

    // if the process exit, the LOCK will be unlock automatically.
    if (flock(fd, LOCK_UN) == -1)
    {
        printf("unlock failed.\n");
        exit(1);
    }

    printf("unlocked of file %s\n", file_name);
    exit(0);
}