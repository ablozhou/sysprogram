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

int fdSingleLane = -1;      // single lane fifo read descriptor
int fdSingleLaneDummy = -1; // single lane fifo dummy write descriptor

// clean up resources
void cleanup(void)
{
  int status;
  if (fdSingleLane != -1)
  {
    status = close(fdSingleLane);
    if (status != 0)
    {
      printf("Error close fifo fd %d", status);
    }
  }
  if (fdSingleLaneDummy != -1)
  {
    status = close(fdSingleLaneDummy);
    if (status != 0)
    {
      printf("Error close fifo dummy fd %d", status);
    }
  }

  status = unlink(SINGLE_LANE);
  if (status != 0)
  {
    printf("Error unlink fifo %s, %d", SINGLE_LANE, status);
  }
}

// Signal handleer when press CTRL+C to exit and clean up
void signal_handler(int signal)
{
  exit(0);
}

void main(int argc, char *argv[])
{
  int status;
  Entrance ent;
  int nextVehicle = 1;
  int nRead;

  // Register clean up function
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
  // Create FIFO
  status = mkfifo(SINGLE_LANE, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (status != 0)
  {
    printf("Error creating fifo %d", status);
    exit(1);
  }

  // Open the fifo to read
  printf("Opening single lane bridge, waiting vehicles.\n");
  fdSingleLane = open(SINGLE_LANE, O_RDONLY);
  if (fdSingleLane == -1)
  {
    printf("Error opening fifo %s.", SINGLE_LANE);
    exit(1);
  }

  // Open the fifo dummy to write to avoid exit of the bridge FIFO for continue accept vehicles.
  fdSingleLaneDummy = open(SINGLE_LANE, O_WRONLY);
  if (fdSingleLaneDummy == -1)
  {
    printf("Error opening fifo dummy %s.", SINGLE_LANE);
    exit(1);
  }

  // Loop to accept vehicles.
  while (TRUE)
  {
    nRead = read(fdSingleLane, &ent, sizeof(ent));
    if (nRead != sizeof(ent))
    {
      printf("Error reading entry.\n");
      exit(1);
    }
    printf("%d: Entrance %d vehicle: %d entering bridge.\n", nextVehicle, ent.entId, ent.vehicleId);
    sleep(1); // Wait for vehicle to enter the single lane bridge
    // printf("%d: left bridge.\n", nextVehicle);
    nextVehicle++;
  }
}