/*
 interactively experiment with record locking. This program takes a single command-line argument: the name of a file on which we wish to place locks. Using this program, we can verify many of our previous statements regarding the operation of record locking. The program is designed to be used interactively and accepts commands of this form:

    cmd lock start length [ whence ]

'cmd' is 'g' (GETLK), 's' (SETLK), or 'w' (SETLKW)
'lock' is 'r' (READ), 'w' (WRITE), or 'u' (UNLOCK)
'start' and 'length' specify byte range to lock
'whence' is 's' (SEEK_SET, default), 'c' (SEEK_CUR), or 'e' (SEEK_END)
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
#include <stdarg.h>
#define MAX_LINE 100
void errExit(char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    exit(1);
}
static void
displayCmdFmt(void)
{
    printf("\n Format: cmd lock start length [whence]\n\n");
    printf("'cmd' is 'g' (GETLK), 's' (SETLK), or 'w' (SETLKW)\n");
    printf("'lock' is 'r' (READ), 'w' (WRITE), or 'u' (UNLOCK)\n");
    printf("'start' and 'length' specify byte range to lock\n");
    printf("'whence' is 's' (SEEK_SET, default), 'c' (SEEK_CUR), "
           "or 'e' (SEEK_END)\n\n");
}

int main(int argc, char *argv[])
{
    int fd, numRead, cmd, status;
    char lock, cmdCh, whence, line[MAX_LINE];
    struct flock fl;
    long long len, st;
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        errExit("Usage: %s file\n", argv[0]);
    fd = open(argv[1], O_RDWR);
    if (fd == -1)
        errExit("open (%s)", argv[1]);
    printf("Enter ? for help\n");
    for (;;)
    { /* Prompt for locking command and carry it out */
        printf("PID=%ld> ", (long)getpid());
        fflush(stdout);
        if (fgets(line, MAX_LINE, stdin) == NULL) /* EOF */
            exit(EXIT_SUCCESS);
        line[strlen(line) - 1] = '\0';
        if (*line == '\0')
            continue;
        if (line[0] == '?')
        {
            displayCmdFmt();
            continue;
        }
        whence = 's';
        /* Remove trailing '\n' */
        /* Skip blank lines */
        /* In case not otherwise filled in */
        numRead = sscanf(line, "%c %c %lld %lld %c", &cmdCh, &lock, &st, &len, &whence);
        fl.l_start = st;
        fl.l_len = len;
        if (numRead < 4 || strchr("gsw", cmdCh) == NULL ||
            strchr("rwu", lock) == NULL || strchr("sce", whence) == NULL)
        {
            printf("Invalid command!\n");
            continue;
        }
        cmd = (cmdCh == 'g') ? F_GETLK : (cmdCh == 's') ? F_SETLK
                                                        : F_SETLKW;
        fl.l_type = (lock == 'r') ? F_RDLCK : (lock == 'w') ? F_WRLCK
                                                            : F_UNLCK;
        fl.l_whence = (whence == 'c') ? SEEK_CUR : (whence == 'e') ? SEEK_END
                                                                   : SEEK_SET;

        // Using fcntl() we can place a lock on any part of a file, ranging from a single byte to the entire file.
        // struct flock flockstr;
        // fcntl(fd, cmd, &flockstr);
        status = fcntl(fd, cmd, &fl);
        if (cmd == F_GETLK)
        {
            if (status == -1)
            {
                errExit("fcntl - F_GETLK");
            }
            else
            {
                if (fl.l_type == F_UNLCK)
                    printf("[PID=%ld] Lock can be placed\n", (long)getpid());
                else /* Locked out by someone else */
                    printf("[PID=%ld] Denied by %s lock on %lld:%lld "
                           "(held by PID %ld)\n",
                           (long)getpid(), (fl.l_type == F_RDLCK) ? "READ" : "WRITE", (long long)fl.l_start,
                           (long long)fl.l_len, (long)fl.l_pid);
            }
        }
        else
        {
            /* F_SETLK, F_SETLKW */
            if (status == 0)
                printf("[PID=%ld] %s\n", (long)getpid(),
                       (lock == 'u') ? "unlocked" : "got lock");
            else if (errno == EAGAIN || errno == EACCES) /* F_SETLK */
                printf("[PID=%ld] failed (incompatible lock)\n", (long)getpid());
            else if (errno == EDEADLK) /* F_SETLKW */
                printf("[PID=%ld] failed (deadlock)\n", (long)getpid());
            else
                errExit("fcntl - F_SETLK(W)");
        }
    }
}