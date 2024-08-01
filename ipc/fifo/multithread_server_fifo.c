#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>  //mkfifo
#include <sys/types.h> //mode_t
#include <pthread.h>

#define SERVER_FIFO "/home/zhh/fifosvr"
#define CLIENT_NAME_TEMPLATE "/home/zhh/fifoclient%d"
#define CLIENT_MAX_NAME_SIZE 128
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
// typedef struct
// {
//   Request *req;
//   unsigned int next_ticket;
// } Context;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
unsigned int next_ticket = 1;

void cleanup(void)
{
  int status = unlink(SERVER_FIFO);
  if (status == -1)
  {
    printf("unlink failed: %s.\n", SERVER_FIFO);
  }
  printf("unlink succeeded: %s.\n", SERVER_FIFO);
}

char *calcFifoName(char *buf, int id)
{
  snprintf(buf, CLIENT_MAX_NAME_SIZE, CLIENT_NAME_TEMPLATE, id);
  return buf;
}
void signal_handler(int signal)
{
  exit(0);
}
void *process(void *context)
{
  printf("Enter thread\n");
  int status;
  int fdClient;
  int nWrite;
  char clientName[CLIENT_MAX_NAME_SIZE];
  Response response;
  Request request = *(Request *)context;
  // free(context);
  printf("context: clientid:%d, tickets: %d.\n", request.clientid, request.tickets);

  status = pthread_mutex_lock(&mutex);
  if (status != 0)
  {
    printf("Error lock.\n");
    exit(1);
  }
  printf("next ticket before: %d\n", next_ticket);
  response.ticket_from = next_ticket;
  next_ticket += request.tickets;
  response.ticket_to = next_ticket - 1;
  printf("next ticket after: %d\n", next_ticket);
  printf("response: from %d to %d\n", response.ticket_from, response.ticket_to);
  status = pthread_mutex_unlock(&mutex);
  if (status != 0)
  {
    printf("Error unlock.\n");
    exit(1);
  }

  calcFifoName(clientName, request.clientid);

  // open client fifo to write
  fdClient = open(clientName, O_WRONLY);
  if (fdClient == -1)
  {
    printf("Failed to open client fifo:%s.\n", clientName);
    // return NULL;
    exit(1);
  }
  // write response
  nWrite = write(fdClient, &response, sizeof(Response));
  if (nWrite != sizeof(Response))
  {
    printf("write to client fifo %s failed.\n", clientName);
    exit(1);
  }
  // close client fifo
  status = close(fdClient);
  if (status == -1)
  {
    printf("Error close fifo: %s.\n", clientName);
    exit(1);
  }

  return NULL;
}

void main(int argc, char *argv[])
{
  int status;
  int fdServer, fdServerDummy;
  int nRead;

  // register atexit to clean
  status = atexit(cleanup);
  if (status != 0)
  {
    printf("Error register atexit.\n");
    exit(1);
  }
  // registe signal
  if (signal(SIGINT, signal_handler) == SIG_ERR)
  {
    printf("Error register signal.\n");
    exit(1);
  }
  // creage fifo
  status = mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
  if (status == -1)
  {
    printf("create fifo failed: %s.\n", SERVER_FIFO);
    exit(1);
  }
  // open fifo to read
  fdServer = open(SERVER_FIFO, O_RDONLY);
  if (fdServer == -1)
  {
    printf("open fifo failed: %s.\n", SERVER_FIFO);
    exit(1);
  }

  // open dummy fifo to write to avoid EOF and exit
  fdServerDummy = open(SERVER_FIFO, O_WRONLY); // WARNING, if O_WRONLY change to O_RDONLY, read will not block after get the first msg.
  if (fdServerDummy == -1)
  {
    printf("open dummy fifo failed: %s.\n", SERVER_FIFO);
    exit(1);
  }
  // loop read from client fifo
  while (TRUE)
  {
    pthread_t thread;
    // Request *request = malloc(sizeof(Request));
    // Request request; // may cause problems
    Request *request = malloc(sizeof(Request)); // Allocate a new Request for each iteration
    if (!request)
    {
      perror("Failed to allocate memory for request");
      exit(EXIT_FAILURE);
    }
    // if there are two reads in the same FIFO SERVER_FIFO, the read will not block.
    nRead = read(fdServer, request, sizeof(Request));
    if (nRead == -1)
    {
      printf("Reading from fifo %s failed.\n", SERVER_FIFO);
      free(request);
      exit(1);
    }
    if (nRead < sizeof(Request))
    {
      free(request); // Handle incomplete read
      continue;      // Optionally handle this more gracefully
    }

    printf("Reading from fifo %s, clientid:%d, tickets: %d.\n", SERVER_FIFO, request->clientid, request->tickets);
    // create thread to response.
    status = pthread_create(&thread, NULL, process, (void *)request);
    if (status == -1)
    {
      printf("pthread_create failed: %s.\n", SERVER_FIFO);
      free(request);
      exit(1);
    }
    pthread_detach(thread);
  }
  // not need pthread join
}