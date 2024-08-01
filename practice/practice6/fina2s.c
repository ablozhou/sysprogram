/*
INSTRUCTIONS Using "File -> Open Folder", open your text editor (Visual Studio Code or Atom) to an empty directory named FinalExam. This should be the top level directory in your editor's file explorer, and it should not contain any files other than the ones you write during this examination. All code in this exam must be written from scratch entirely by you. If you submit any existing code in any form written by anyone, then you will receive an F grade in the entire course. Use of the "man" command is allowed.
PROBLEM STATEMENT Create a FIFO-based client server application consisting of two programs, client.c and server.c. The client will send a request to the server containing three fields, namely, its process ID (i.e., PID), a string message (with at most 100 bytes), and a string mode whose value can be "upper" or "lower". On receiving a request, the server will send a response to the client containing the message transformed to all uppercase or all lowercase characters depending on whether the mode field in the request is "upper" or "lower". On receiving a response, the client should print it, remove its response FIFO, and exit. The server must be multithreaded and must be able to handle multiple requests arriving at the same time. For every request received by the server, a new child thread must be created to compute and send the response. To compute the response, a visitor (implemented using function pointer) must be applied to the characters in the message. The visitor must be set to the toupper or tolower functions defined in ctype.h depending on whether the mode field in the request is "upper" or "lower". The request FIFO should be named RequestFIFO and should be created by the server when the server is launched, and removed by the server when the server exits. The server must exit upon receiving CTRL+C. Each client should create its response FIFO for receiving the response and should remove its response FIFO before exiting. If the client's PID is n, then its response FIFO should be named ResponseFifo.n (e.g., if the client's PID is 7645 then its response FIFO should be named ResponseFifo.7645). COMPILING The two programs should be compiled as follows: cc server.c -o server -pthread cc client.c -o client RUNNING server The server should be run as follows: ./server client The client should be run as follows: ./client message mode For example, running ./client LinuxProgramming upper should produce the following output (printed by the client) LINUXPROGRAMMING and running ./client LinuxProgramming lower should produce the output (printed by the client) linuxprogramming If the number of command line arguments is not correct or if the user runs "./client -h" or "./client --help", then the client should print
Usage: ./client message mode If mode is not "upper" or "lower", then the client should print mode must be upper or lower. If client fails to send a request to the server (e.g., because the server is not up and running), then it should print Client cannot reach server. WHAT TO SUBMIT Copy the client and server programs to files with .txt extension using the following commands cp client.c client.txt cp server.c server.txt Submit the files client.txt and server.txt
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>

#define TRUE 1
#define MSG_LEN 100
#define BUFSIZE 1024
#define UPPER "upper"
#define LOWER "lower"
// #define FIFO "RequestFIFO"
// #define CLIENT_FIFO_TMPL "ResponseFifo.%d"
#define FIFO "/home/zhh/RequestFIFO"
#define CLIENT_FIFO_TMPL "/home/zhh/ResponseFifo.%d"

typedef int (*visitor)(int);
int fds = -1, fdDummy = -1;

typedef struct
{
    pid_t pid;
    char msg[MSG_LEN];
    char mode[10]; // upper,lower
} Request;
typedef struct
{
    char msg[MSG_LEN];
} Response;

void cleanup()
{
    // close fd
    if (fdDummy > 0)
    {
        if (close(fdDummy) == -1)
        {
            printf("Close fd dummy failed.\n");
        }
    }
    if (fds > 0)
    {
        if (close(fds) == -1)
        {
            printf("Close fd server failed.\n");
        }
    }
    // unlink fifo
    if (unlink(FIFO) == -1)
    {
        printf("unlink fifo %s failed: %s.\n", FIFO, strerror(errno));
    }
    else
    {
        printf("unlink %s succeeded.\n", FIFO);
    }
}
void signal_handler(int signal)
{
    exit(0);
}

void *process(void *data)
{
    int status;
    char buffer[BUFSIZE];
    int nWrite;
    if (data == NULL)
    {
        printf("Error: data is NULL\n");
        exit(EXIT_FAILURE);
    }
    Request *req = (Request *)data;
    // open fifo
    snprintf(buffer, BUFSIZE, CLIENT_FIFO_TMPL, req->pid);
    int fd = open(buffer, O_WRONLY);
    if (fd == -1)
    {
        printf("Error: open %s failed.\n", buffer);
        exit(EXIT_FAILURE);
    }

    visitor vist = strcmp(req->mode, "upper") == 0 ? toupper : tolower;
    Response res;

    int len = strlen(req->msg);
    for (int i = 0; i < len; i++)
    {
        res.msg[i] = vist(req->msg[i]);
    }
    // write fifo
    printf("send to %s: %d, %s.\n", buffer, req->pid, res.msg);
    nWrite = write(fd, &res, sizeof(Response));
    if (nWrite != sizeof(Response))
    {
        printf("Error: write failed.\n");
        exit(EXIT_FAILURE);
    }
    status = close(fd);
    if (status == -1)
    {
        printf("Error: close failed.\n");
        exit(EXIT_FAILURE);
    }

    return NULL;
}

void main(int argc, char *argv[])
{
    int status;
    char buffer[BUFSIZE];
    int nRead;
    pthread_t thd;

    Request req;
    status = atexit(cleanup);
    if (status != 0)
    {
        printf("Error reg exit failed.\n");
        exit(1);
    }
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        printf("Error reg sig handler failed.\n");
        exit(1);
    }
    // mkfifo
    status = mkfifo(FIFO, 0644);
    if (status == -1)
    {
        printf("Error: mkfifo failed.\n");
        exit(EXIT_FAILURE);
    }
    // open fifo
    int fd = open(FIFO, O_RDONLY);
    if (fd == -1)
    {
        printf("Error: open failed.\n");
        exit(EXIT_FAILURE);
    }
    int fdDummy = open(FIFO, O_WRONLY);
    if (fdDummy == -1)
    {
        printf("Error: open fdDummy failed.\n");
        exit(EXIT_FAILURE);
    }
    while (TRUE)
    {
        // read
        nRead = read(fd, &req, sizeof(Request));
        if (nRead == -1)
        {
            printf("Error: read failed.\n");
            exit(EXIT_FAILURE);
        }
        printf("received:%d, %s, %s\n", req.pid, req.msg, req.mode);
        status = pthread_create(&thd, NULL, process, &req);
        if (status == -1)
        {
            printf("Error: ptherad_create failed.\n");
            exit(EXIT_FAILURE);
        }
        status = pthread_detach(thd);
        if (status == -1)
        {
            printf("Error: pthread_detach failed.\n");
            exit(EXIT_FAILURE);
        }
    }
}