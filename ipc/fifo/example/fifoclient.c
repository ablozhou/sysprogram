#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
// cat requests.txt|xargs -n 1 -P 8 ./client
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

/* Create exit handler for removing response FIFO */
void removeResponseFifo()
{
  int status;
  char responseFifoName[MAX_LENGTH];

  snprintf(responseFifoName, MAX_LENGTH, CLIENT_FIFO_TEMPLATE, getpid());

  status = unlink(responseFifoName);
  if (status == -1)
  {
    printf("Client failed to remove response FIFO.\n");
  }
}

int main(int argc, char *argv[])
{
  int numTickets, status, requestFifoDescriptor, responseFifoDescriptor;
  Request request;
  Response response;
  int numRead, numWritten;
  char responseFifoName[MAX_LENGTH];

  if (argc != 2 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
  {
    printf("Usage: %s <number of tickets>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  numTickets = atoi(argv[1]);
  if (numTickets <= 0)
  {
    printf("Number of tickets must be positive.\n");
    exit(EXIT_FAILURE);
  }

  /* Register the exit handler */
  status = atexit(removeResponseFifo);
  if (status != 0)
  {
    printf("Client was unable to register exit handler.\n");
    exit(EXIT_FAILURE);
  }

  /* Create the response FIFO */
  snprintf(responseFifoName, MAX_LENGTH, CLIENT_FIFO_TEMPLATE, getpid());

  status = mkfifo(
      responseFifoName,
      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
  if (status == -1)
  {
    printf("Client failed to create response FIFO.\n");
    exit(EXIT_FAILURE);
  }

  /* Prepare the request */
  request.clientId = getpid();
  request.numTickets = (unsigned)numTickets;

  /* Put the request on the request FIFO */
  requestFifoDescriptor = open(SERVER_FIFO_NAME, O_WRONLY);
  if (requestFifoDescriptor == -1)
  {
    printf("Client cannot reach server.\n");
    exit(EXIT_FAILURE);
  }

  numWritten = write(requestFifoDescriptor, &request, sizeof(Request));
  if (numWritten < sizeof(Request))
  {
    printf("Client encountered partial write error while writing the request.\n");
    exit(EXIT_FAILURE);
  }

  status = close(requestFifoDescriptor);
  if (status == -1)
  {
    printf("Client failed to close the request FIFO.\n");
    exit(EXIT_FAILURE);
  }

  /* Read the response */
  responseFifoDescriptor = open(responseFifoName, O_RDONLY);
  if (responseFifoDescriptor == -1)
  {
    printf("Client failed to open response FIFO.\n");
    exit(EXIT_FAILURE);
  }

  numRead = read(responseFifoDescriptor, &response, sizeof(Response));
  if (numRead == -1)
  {
    printf("Client failed to read response.\n");
    exit(EXIT_FAILURE);
  }

  printf(
      "First ticket = %u, last ticket = %u\n",
      response.firstTicket,
      response.lastTicket);

  status = close(responseFifoDescriptor);
  if (status == -1)
  {
    printf("Client failed to close response FIFO.\n");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}