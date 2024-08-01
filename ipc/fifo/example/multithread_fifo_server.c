#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
// seq 1 10 | xargs -n 1 -p 0 ./clent
/* Define the protocol */

typedef struct
{
  pid_t clientId;
  unsigned numTickets;
} Request;

typedef struct
{
  unsigned firstTicket;
  unsigned lastTicket;
} Response;

/* FIFO names */

#define SERVER_FIFO_NAME "RequestFifo"
#define CLIENT_FIFO_TEMPLATE "ResponseFifo.%d"

#define MAX_LENGTH 100

/* Create exit handler for removing request FIFO */
void removeRequestFifo()
{
  int status;

  status = unlink(SERVER_FIFO_NAME);
  if (status == -1)
  {
    printf("Server failed to remove request FIFO.\n");
  }
}

/* Create signal handler for CTRL+C */
void handleInterruptSignal(int signal)
{
  exit(EXIT_SUCCESS);
}

volatile unsigned nextTicket = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *giveOutTickets(void *requestPointer)
{
  int status, responseFifoDescriptor, numWritten;
  Response response;
  char responseFifoName[MAX_LENGTH];

  Request request = *(Request *)requestPointer;
  printf(
      "Server received a request from client %d for %u tickets.\n",
      request.clientId,
      request.numTickets);

  /* Compute the response */
  status = pthread_mutex_lock(&mutex);
  if (status != 0)
  {
    printf("Failed to lock mutex.\n");
    exit(EXIT_FAILURE);
  }

  response.firstTicket = nextTicket;
  response.lastTicket =
      response.firstTicket + request.numTickets - 1;
  nextTicket = response.lastTicket + 1;

  status = pthread_mutex_unlock(&mutex);
  if (status != 0)
  {
    printf("Failed to unlock mutex.\n");
    exit(EXIT_FAILURE);
  }

  /* Send the response to the correct client */

  /* Calculate the NAME of the response FIFO */
  snprintf(
      responseFifoName,
      MAX_LENGTH,
      CLIENT_FIFO_TEMPLATE,
      request.clientId);

  /* Open the response FIFO */
  responseFifoDescriptor = open(responseFifoName, O_WRONLY);
  if (responseFifoDescriptor == -1)
  {
    printf("Server failed to open response FIFO.\n");
    exit(EXIT_FAILURE);
  }

  /* Write the response */
  numWritten = write(
      responseFifoDescriptor,
      &response,
      sizeof(Response));
  if (numWritten < sizeof(Response))
  {
    printf("Server encountered partial write error.\n");
    exit(EXIT_FAILURE);
  }

  /* Close the response FIFO so that client can delete it */
  status = close(responseFifoDescriptor);
  if (status == -1)
  {
    printf("Server failed to close response FIFO.\n");
    exit(EXIT_FAILURE);
  }

  free(requestPointer);

  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  int status, requestFifoDescriptor, dummyDescriptor;
  Request request;
  int numRead;
  pthread_t requestHandlerThread;
  Request *requestHandlerInput;

  /* Register the exit handler */
  status = atexit(removeRequestFifo);
  if (status != 0)
  {
    printf("Server was unable to register exit handler.\n");
    exit(EXIT_FAILURE);
  }

  /* Register signal handler for CTRL+C */
  if (signal(SIGINT, handleInterruptSignal) == SIG_ERR)
  {
    printf("Server failed to register handler for CTRL+C\n");
    exit(EXIT_FAILURE);
  }

  /* Create the request FIFO */
  status = mkfifo(
      SERVER_FIFO_NAME,
      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
  if (status == -1)
  {
    printf("Server failed to create request FIFO.\n");
    exit(EXIT_FAILURE);
  }

  /* Open the request FIFO for reading */
  requestFifoDescriptor = open(SERVER_FIFO_NAME, O_RDONLY);
  if (requestFifoDescriptor == -1)
  {
    printf("Server failed to open request FIFO for reading.\n");
    exit(EXIT_FAILURE);
  }

  /* Open the dummy descriptor to avoid getting EOF */
  dummyDescriptor = open(SERVER_FIFO_NAME, O_WRONLY);
  if (dummyDescriptor == -1)
  {
    printf("Server failed to open the dummy descriptor.\n");
    exit(EXIT_FAILURE);
  }

  while (TRUE) /* can also be written as for(;;) */
  {
    /* Read the next request */
    numRead = read(requestFifoDescriptor, &request, sizeof(Request));
    if (numRead == -1)
    {
      printf("Server failed to read request.\n");
      exit(EXIT_FAILURE);
    }

    /* Create the request handler input
       so that every child thread gets its own request object
       instead of reusing a single request variable
       which would lead to a race condition
    */
    requestHandlerInput = (Request *)malloc(sizeof(Request));
    if (requestHandlerInput == NULL)
    {
      printf("Failed to allocate memory.\n");
      exit(EXIT_FAILURE);
    }

    memcpy(requestHandlerInput, &request, sizeof(Request));

    /* Create request handler thread */
    status = pthread_create(
        &requestHandlerThread,
        NULL,
        giveOutTickets,
        (void *)requestHandlerInput);
    if (status != 0)
    {
      printf("Failed to create thread.\n");
      exit(EXIT_FAILURE);
    }

    /* Detach the request handler thread, not using pthread_join, because need not  wait, there will be no parellel */
    status = pthread_detach(requestHandlerThread);
    if (status != 0)
    {
      printf("Failed to detach thread.\n");
      exit(EXIT_FAILURE);
    }
  }

  exit(EXIT_SUCCESS);
}