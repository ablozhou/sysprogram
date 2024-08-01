/*In this project, we will develop a traffic-control system, using mutexes, condition variables, FIFO and semaphores in the Linux OS system to simulate it.
The intersection traffic system using mutex and Condition variables,  a single-lane bridge traffic control using FIFO and mutex, and
a parking lot using semophore, mutex and condition variables.
This simulator traffic control system is programmed in C and tested on Ubuntu using
multi-thread or multi-process techniques to help understanding the mechanisms in
concurrent scenarios when access sharing resources.

Using FIFO named queue to simulate the single-lane traffic control
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#define SINGLE_LANE "/home/zhh/fifo_singlelane"
#define TRUE 1

typedef struct
{
  unsigned int entId;
  unsigned int vehicleId;
} Entrance;

int fdSingleLane = -1;
void cleanup(void)
{
  int status;
  if (fdSingleLane != -1)
  {
    status = close(fdSingleLane);
    if (status != 0)
    {
      printf("Error %d close fifo writing fd.", status);
    }
  }
}

// Handle Ctrl+c for exit and clean
void signal_handler(int signal)
{
  exit(0);
}

void main(int argc, char *argv[])
{
  int status;
  unsigned int entId = 0;
  int nextVehicle = 1;
  int nRead;

  if (argc < 2 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
  {
    printf("Usage: %s entranceId\n", argv[0]);
    exit(1);
  }
  entId = (unsigned int)atoi(argv[1]);

  status = atexit(cleanup);
  if (status != 0)
  {
    printf("Error register cleanup %d", status);
    exit(1);
  }
  // Register CTRL-C signal handlers
  if (signal(SIGINT, signal_handler) == SIG_ERR)
  {
    printf("Error register signal.\n");
    exit(1);
  }
  // Open the fifo to write
  fdSingleLane = open(SINGLE_LANE, O_WRONLY);
  if (fdSingleLane == -1)
  {
    printf("Error opening fifo %s.", SINGLE_LANE);
    exit(1);
  }

  while (TRUE)
  {
    Entrance ent;
    ent.entId = entId;
    ent.vehicleId = nextVehicle;
    nRead = write(fdSingleLane, &ent, sizeof(ent));
    if (nRead != sizeof(ent))
    {
      printf("Error reading entry.\n");
      exit(1);
    }
    printf("Entrance %d vehicle: %d entering single lane.\n", ent.entId, ent.vehicleId);
    sleep(1);

    nextVehicle++;
  }
}