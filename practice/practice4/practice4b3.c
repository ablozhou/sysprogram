/*
3. Implement a program in which two unrelated processes, a writer and a reader, communicate using a FIFO. The writer creates a FIFO and writes a string of any length (up to a maximum) to the FIFO, whereas the reader reads that string from the FIFO and prints out the length of the string after which it deletes the FIFO. Test this program out with the following strings:

    (a) Hello World! The length should be 12.
    (b) Linux Programming The length should be 17.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

#define TRUE 1
#define MAX_SIZE 1024

#define FIFO "/home/zhh/myfifo"

#define MODE 0664

int fdSvrReader = 0;
int fdDummyWriter = 0;
void cleanup(void)
{
    int status;
    if (fdSvrReader != 0)
    {
        status = close(fdSvrReader);
        // todo check
        printf("fd %d closed.\n", fdSvrReader);
    }
    if (fdDummyWriter != 0)
    {
        status = close(fdDummyWriter);
        // todo check
        printf("fd %d closed.\n", fdDummyWriter);
    }
    if (unlink(FIFO) != 0)
    {
        printf("unlink %s failed\n", FIFO);
    }
    else
    {
        printf("unlink %s succeeded\n", FIFO);
    }
}

void signal_handler(int signal)
{
    exit(0);
}

void server()
{
    int status;
    int nRead;

    char buffer[MAX_SIZE];

    // register clean up
    status = atexit(cleanup);
    // toto check
    // register signal
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        printf("signal_handler register failed.\n");
        exit(1);
    }

    // mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
    mode_t mode = MODE;
    status = mkfifo(FIFO, mode);
    // check
    if (status == -1)
    {
        printf("mkfifo failed.\n");
        exit(1);
    }
    printf("mkfifo succeeded %s.\n", FIFO);
    // open
    fdSvrReader = open(FIFO, O_RDONLY);
    if (fdSvrReader <= 0)
    {
        printf("Error opening fifo to read %s\n", FIFO);
        exit(1);
    }
    fdDummyWriter = open(FIFO, O_WRONLY);
    if (fdDummyWriter <= 0)
    {
        printf("Error opening fifo dummy write %s\n", FIFO);
        exit(1);
    }
    printf("waiting message...\n");
    // read
    while (TRUE)
    {
        nRead = read(fdSvrReader, buffer, MAX_SIZE - 1);
        buffer[nRead] = '\0';
        printf("%s The length should be %d\n", buffer, nRead);
        sleep(2);
    }
}
void client(char *buffer)
{
    int status, fd;
    int nWrite = 0;
    fd = open(FIFO, O_WRONLY | O_APPEND);
    if (fd < 0)
    {
        printf("Error opening fifo to write %s\n", FIFO);
        exit(1);
    }
    nWrite = write(fd, buffer, strlen(buffer));
    if (nWrite != strlen(buffer))
    {
        printf("Error writing to fifo %s\n", FIFO);
        exit(1);
    }
    status = close(fd);
    // todo check
}
void main(int argc, char *argv[])
{

    int status;
    if (argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        printf("Usage: %s -s | -h | --help | <\"string arg\"> \n", argv[0]);
        exit(1);
    }

    if (strcmp(argv[1], "-s") == 0)
    {
        server();
    }
    else
    {
        client(argv[1]);
    }

    // close
    // status = close(fd);
    //  todo check
}