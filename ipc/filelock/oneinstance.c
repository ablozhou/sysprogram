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
FD_CLOEXEC is actually the only bit used in the file descriptor flags. This bit corre- sponds to the value 1. In older programs, we may sometimes see the close-on- exec flag set using just the call fcntl(fd, F_SETFD, 1), relying on the fact that there are no other bits that can be affected by this operation. Theoretically, this may not always be so (in the future, some UNIX system might implement additional flag bits), so we should use the technique shown in the main text.
Many UNIX implementations, including Linux, also allow the close-on- exec flag to be modified using two unstandardized ioctl() calls: ioctl(fd, FIOCLEX) to set the close-on-exec flag for fd, and ioctl(fd, FIONCLEX) to clear the flag.


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
#include <stdarg.h>

#define CPF_CLOEXEC 1
#define BUF_SIZE 100

void errExit(char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    exit(1);
}

static int
lockRegion(int fd, int cmd, int type, int whence, int start, off_t len)
{
    struct flock fl;

    fl.l_type = type;
    fl.l_whence = whence;
    fl.l_start = start;
    fl.l_len = len;

    return fcntl(fd, cmd, &fl);
}

int createPidFile(const char *progName, const char *pidFile, int flags)
{
    int fd;
    char buf[BUF_SIZE];

    fd = open(pidFile, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
        errExit("Could not open PID file %s\n", pidFile);

    // if (flags & CPF_CLOEXEC)
    // {

    //     /* Set the close-on-exec file descriptor flag */

    //     flags = fcntl(fd, F_GETFD); /* Fetch flags */
    //     if (flags == -1)
    //         errExit("Could not get flags for PID file %s\n", pidFile);

    //     flags |= FD_CLOEXEC; /* Turn on FD_CLOEXEC */

    //     if (fcntl(fd, F_SETFD, flags) == -1) /* Update flags */
    //         errExit("Could not set flags for PID file %s\n", pidFile);
    // }

    // F_WRLCK will not block and failed immediately.
    if (lockRegion(fd, F_SETLK, F_WRLCK, SEEK_SET, 0, 0) == -1)
    {
        if (errno == EAGAIN || errno == EACCES)
            errExit("PID file '%s' is locked; probably "
                    "'%s' is already running.\n",
                    pidFile, progName);
        else
            errExit("Unable to lock PID file '%s'\n", pidFile);
    }

    if (ftruncate(fd, 0) == -1)
        errExit("Could not truncate PID file '%s'\n", pidFile);

    snprintf(buf, BUF_SIZE, "%ld\n", (long)getpid());
    if (write(fd, buf, strlen(buf)) != strlen(buf))
        errExit("Writing to PID file '%s'\n", pidFile);

    return fd;
}
void main(int argc, char *argv[])
{
    int status;
    if (argc < 3)
    {
        printf("Usage: %s pid_file sleep_time(s)\n", argv[0]);
        exit(1);
    }
    char *pid_file = argv[1];
    int fd = createPidFile(argv[0], pid_file, 0);
    if (fd == -1)
    {
        printf("Error createPidFile\n");
        exit(1);
    }
    sleep(atoi(argv[2]));
    close(fd);
    exit(0);
}