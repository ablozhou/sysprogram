#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>  //mkfifo
#include <sys/types.h> //mode_t

#define SERVER_FIFO "/home/zhh/fifosvr"
#define CLIENT_NAME_TEMPLATE "/home/zhh/fifoclient%d"
#define CLIENT_MAX_SIZE 128
#define TRUE 1

char *clientFifo = NULL;

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
  if (clientFifo != NULL)
  {
    int status = unlink(clientFifo);
    if (status == -1)
    {
      printf("Unlink %s failed: %d\n", clientFifo, status);
    }
    printf("Unlink %s successful\n", clientFifo);
  }
}
void signal_handler(int signal)
{
  cleanup();
  exit(1);
}

void main(int argc, char *argv[])
{
  int status;
  int nRead, nWrite;
  int fdServer, fdClient;
  Request request;
  Response response;

  char clientName[CLIENT_MAX_SIZE];
  int clientid, ticketsNumber;

  if (argc < 3 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
  {
    printf("Usage: %s clientid ticketsNum.\n", argv[0]);
    exit(1);
  }
  clientid = atoi(argv[1]);
  ticketsNumber = atoi(argv[2]);
  // register signal
  if (signal(SIGINT, signal_handler) == SIG_ERR)
  {
    printf("Register signal SIGINT error.\n");
    exit(1);
  }
  if (signal(SIGTERM, signal_handler) == SIG_ERR)
  {
    printf("Register signal SIGTERM error.\n");
    exit(1);
  }
  // get client name
  snprintf(clientName, CLIENT_MAX_SIZE, CLIENT_NAME_TEMPLATE, clientid);
  // set global client fifo
  clientFifo = clientName;
  // create client fifo
  fdClient = mkfifo(clientName, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
  if (fdClient == -1)
  {
    printf("Failed to create client fifo:%s\n", clientName);
    exit(1);
  }
  // When you open a FIFO (named pipe) in Linux, the behavior of the open() system call can indeed cause the calling process to halt (block) under certain conditions,
  // open server fifo will block until the server open to read.
  printf("Open server to write: %s\n", SERVER_FIFO);
  fdServer = open(SERVER_FIFO, O_WRONLY);
  if (fdServer == -1)
  {
    printf("Failed to open server fifo\n");
    exit(1);
  }
  // open client fifo, this will block until server open client to write. but if you don't send, the server will no open client fifo, so there will be deadlock.
  // this open to read must move below the write.
  // printf("Open client to read: %s\n", clientName);
  // fdClient = open(clientName, O_RDONLY);
  // if (fdClient == -1)
  // {
  //   printf("Failed to open client fifo\n");
  //   exit(1);
  // }

  // write server fifo
  request.clientid = clientid;
  request.tickets = ticketsNumber;
  printf("Writing tickets to server: %s\n", SERVER_FIFO);
  nWrite = write(fdServer, &request, sizeof(request));
  printf("request size: %ld\n", sizeof(Request));
  if (nWrite < sizeof(request))
  {
    printf("Failed to write server fifo\n");
    exit(1);
  }

  printf("Open client to read: %s\n", clientName);
  fdClient = open(clientName, O_RDONLY);
  if (fdClient == -1)
  {
    printf("Failed to open client fifo\n");
    exit(1);
  }

  printf("Reading tickets to client: %s\n", clientFifo);
  // reading client fifo
  nRead = read(fdClient, &response, sizeof(response));
  if (nRead == -1)
  {
    printf("Failed to read client fifo\n");
    exit(1);
  }
  printf("Client received tickets from %d to %d\n", response.ticket_from, response.ticket_to);

  // close server fifo
  status = close(fdServer);
  if (status == -1)
  {
    printf("Close server fifo failed.\n");
    exit(1);
  }
  // close client fifo
  status = close(fdClient);
  if (status == -1)
  {
    printf("Close client fifo failed.\n");
    exit(1);
  }
  // unlink client fifo
  status = unlink(clientName);
  if (status == -1)
  {
    printf("unlink client fifo %s failed.\n", clientName);
    exit(1);
  }
  exit(0);
}