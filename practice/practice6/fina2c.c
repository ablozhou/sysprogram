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
#define FIFO "/home/zhh/RequestFIFO"
#define CLIENT_FIFO_TMPL "/home/zhh/ResponseFifo.%d"

typedef int (*visitor)(int);
int fdc = -1;
char *client_fifo_name = NULL;
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
    if (fdc > 0)
    {
        if (close(fdc) == -1)
        {
            printf("Close fd server failed.\n");
        }
    }
    // unlink fifo
    if (client_fifo_name != NULL)
    {

        if (unlink(client_fifo_name) == -1)
        {
            printf("unlink fifo %s failed: %s.\n", client_fifo_name, strerror(errno));
        }
        else
        {
            printf("unlink %s succeeded.\n", client_fifo_name);
        }
    }
}
void signal_handler(int signal)
{
    exit(0);
}

void main(int argc, char *argv[])
{
    int status;
    char buffer[BUFSIZE];
    int nWrite;
    Request req;
    Response res;
    if (argc < 3 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        printf("Usage: %s message mode.\n", argv[0]);
        exit(1);
    }

    char *msg = argv[1];
    char *mode = argv[2];
    if (strcmp(mode, UPPER) != 0 && strcmp(mode, LOWER) != 0)
    {
        printf("%d,%d,%d\n", strcmp(mode, "upper") != 0, strcmp(mode, "lower") != 0, strcmp(mode, "upper") != 0 || strcmp(mode, "lower") != 0);
        printf("mode:%s.\n", mode);
        printf("mode must be upper or lower.\n");
        exit(1);
    }

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
    req.pid = getpid();
    strncpy(req.msg, msg, MSG_LEN - 1);
    req.msg[MSG_LEN - 1] = '\0';
    strncpy(req.mode, mode, 10 - 1);
    req.mode[10 - 1] = '\0';
    snprintf(buffer, BUFSIZE, CLIENT_FIFO_TMPL, req.pid);
    client_fifo_name = buffer;
    // mkfifo
    status = mkfifo(client_fifo_name, 0644);
    if (status == -1)
    {
        printf("Error: mkfifo failed.\n");
        exit(EXIT_FAILURE);
    }

    printf("open %s\n", FIFO);
    int fd = open(FIFO, O_WRONLY, 0644);
    if (fd == -1)
    {
        printf("Error: open failed.\n");
        exit(EXIT_FAILURE);
    }
    // write fifo
    printf("write %s: %d,%s,%s\n", FIFO, req.pid, req.msg, req.mode);
    nWrite = write(fd, &req, sizeof(req));
    if (nWrite != sizeof(req))
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
    printf("open %s\n", client_fifo_name);
    // Note: open will block. open client fifo,
    fdc = open(client_fifo_name, O_RDONLY, 0644);
    if (fdc == -1)
    {
        printf("Error: open fdc failed.\n");
        exit(EXIT_FAILURE);
    }
    int nRead = read(fd, &res, sizeof(res));
    if (nRead != sizeof(res))
    {
        printf("Error: read failed.\n");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", res.msg);
    exit(0);
}