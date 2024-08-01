#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>  //mkfifo
#include <sys/types.h> //mode_t

#define SERVER_FIFO "/home/zhh/fifosvr"
#define CLIENT_NAME_TEMPLATE "/home/zhh/fifoclient%d"
#define CLIENT_MAX_SIZE 128
#define TRUE 1

typedef struct
{
  unsigned int clientid;
  unsigned int tickets;
} Request;

typedef struct
{
  unsigned int ticket_from;
  unsigned int ticket_to;
} Response;

void cleanup(void)
{
  int status = unlink(SERVER_FIFO);
  if (status == -1)
  {
    printf("unlink failed: %d\n", status);
    //_exit(1); //?
  }
  printf("unlink %s succeeded.\n", SERVER_FIFO);
}

void signal_handler(int signal)
{
  // cleanup();
  if (signal == SIGINT)
  {
    printf("signal SIGINT: %d received, exiting\n", signal);
    exit(0);
  }
  printf("signal: %d received, do nothing\n", signal);
  // exit(1);
}

void main(int argc, char *argv[])
{
  int status;
  int fdServer, fdDummyWriter, fdClient;
  int nRead, nWrite;
  int nextTicket = 1;
  char clientName[CLIENT_MAX_SIZE];

  Request request;
  Response response;

  // register atexit
  status = atexit(cleanup);
  if (status == -1)
  {
    printf("error: atexit\n");
    exit(1);
  }

  // register signal
  if (signal(SIGINT, signal_handler) == SIG_ERR)
  {
    printf("error: register signal_handler.\n");
    exit(1);
  }

  // create fifo
  status = mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
  if (status == -1)
  {
    printf("mkfifo failed.\n");
    exit(1);
  }

  // open fifo to reading and an dummy write fifo for not closing the server fifo
  printf("Open server to read: %s\n", SERVER_FIFO);
  fdServer = open(SERVER_FIFO, O_RDONLY);
  if (fdServer == -1)
  {
    printf("open fifo to read failed\n");
    exit(1);
  }

  printf("Open dummy server to write: %s\n", SERVER_FIFO);
  fdDummyWriter = open(SERVER_FIFO, O_WRONLY);
  if (fdDummyWriter == -1)
  {
    printf("open fifo to dummy write failed\n");
    exit(1);
  }

  // continue reading and response
  while (TRUE)
  {
    printf("Reading tickets request from server fifo: %s\n", SERVER_FIFO);
    nRead = read(fdServer, &request, sizeof(Request));
    if (nRead == -1)
    { // or != sizeof(Request)
      printf("read from server fifo failed\n");
      exit(1);
    }
    printf("read from server fifo: clientid: %d, tickets: %d\n", request.clientid, request.tickets);
    response.ticket_from = nextTicket;
    nextTicket += request.tickets;
    response.ticket_to = nextTicket - 1;

    // calculate client name
    snprintf(clientName, CLIENT_MAX_SIZE, CLIENT_NAME_TEMPLATE, request.clientid);
    // open client fifo
    printf("Open client fifo %s to write.\n", clientName);
    fdClient = open(clientName, O_WRONLY);
    if (fdClient == -1)
    {
      printf("open client fifo failed\n");
      exit(1);
    }
    printf("Writing tickets from %d to %d to client: %s\n", response.ticket_from, response.ticket_to, clientName);
    status = write(fdClient, &response, sizeof(response));
    if (status != sizeof(response))
    {
      printf("write client fifo failed\n");
      exit(1);
    }
    // close client fifo
    status = close(fdClient);
    if (status == -1)
    {
      printf("close client fifo failed\n");
      exit(1);
    }
  }
}