#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define TRUE 1
#define FALSE 0

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

int main(int argc, char *argv[])
{
  int status, requestFifoDescriptor, responseFifoDescriptor, dummyDescriptor;
  char responseFifoName[MAX_LENGTH];
  Request request;
  Response response;
  int numRead, numWritten;
  unsigned nextTicket = 1;

  /* Register the exit handler */
  status = atexit(removeRequestFifo);
  if (status == -1)
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

    printf(
        "Server received a request from client %d for %u tickets.\n",
        request.clientId,
        request.numTickets);

    /* Compute the response */
    response.firstTicket = nextTicket;
    response.lastTicket =
        response.firstTicket + request.numTickets - 1;
    nextTicket = response.lastTicket + 1;

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
  }

  exit(EXIT_SUCCESS);
}