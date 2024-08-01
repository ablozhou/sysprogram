/*In this project, we will develop a traffic-control system, using mutexes, condition variables, FIFO and semaphores in the Linux OS system to simulate it.
The intersection traffic system using mutex and Condition variables,  a single-lane bridge traffic control using FIFO and mutex, and
a parking lot using semophore, mutex and condition variables.
This simulator traffic control system is programmed in C and tested on Ubuntu using
multi-thread or multi-process techniques to help understanding the mechanisms in
concurrent scenarios when access sharing resources.

Direction Enum: Defines an enum for the eight directions and includes DIRECTION_COUNT to represent the total number of directions.
Mutex Array: Defines an array of mutexes, with one mutex for each direction.
Traffic Controller Thread: The traffic_controller thread rotates the current_direction every minute and broadcasts the condition variable to wake up waiting threads.
Traffic Function: Each direction's traffic function calculates its group based on the direction enum. It waits until its group is allowed to pass and then enters the critical section protected by its corresponding mutex.
Thread Creation and Destruction: In the main function, the traffic controller thread and traffic threads are created, and the program waits for all threads to complete. Finally, all mutexes and the condition variable are destroyed.
This setup ensures fair traffic control, allowing each direction to pass for 1 minute and allowing mutually independent directions to pass simultaneously. Using enums and arrays makes the code cleaner and easier to understand.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
//-east2south(turn right not need red-green light)
// east2west, east2north,
// west2east, west2south
// south2north, south2east
// north2south, north2west

// east2west, west2east at the same time
// east2north,west2south at the same time
// south2north, north2south at the same time
// south2east, north2west at the same time

#define TRUE 1
#define MAX_DIRECTION_SIZE 4

// Define an enum for directions
typedef enum
{
  EAST2WEST,
  WEST2EAST,
  EAST2NORTH,
  WEST2SOUTH,
  SOUTH2NORTH,
  NORTH2SOUTH,
  SOUTH2EAST,
  NORTH2WEST,
  DIRECTION_COUNT // Number of directions
} Direction;

const char *Direction_Names[] = {
    "EAST2WEST",
    "WEST2EAST",
    "EAST2NORTH",
    "WEST2SOUTH",
    "SOUTH2NORTH",
    "NORTH2SOUTH",
    "SOUTH2EAST",
    "NORTH2WEST"};

// Define an array of mutexes to represent the red-green light of each direction.
pthread_mutex_t mutexes[DIRECTION_COUNT];

// Condition variable and schedule mutex
pthread_cond_t cond;            // To wait and notity the lights convert
pthread_mutex_t schedule_mutex; // for lights conditional variable

// Currently allowed direction group
int current_direction = 7; // Ranges from 0 to 3, representing each group of directions, 7 is an illegal group, represents all red light for syncronization.

// Red-green lights controller
void *cross_traffic_controller(void *arg)
{
  int status;
  while (TRUE)
  {
    status = pthread_mutex_lock(&schedule_mutex);
    if (status != 0)
    {
      printf("Error: traffic_controller pthread_mutex_lock failed\n");
      exit(1);
    }
    current_direction = (current_direction + 1) % MAX_DIRECTION_SIZE; // Rotate to the next group
    printf("Green light:%s, %s.\n", Direction_Names[current_direction * 2], Direction_Names[current_direction * 2 + 1]);
    status = pthread_cond_broadcast(&cond); // Wake up all waiting threads
    if (status != 0)
    {
      printf("Error: traffic_controller pthread_cond_broadcast failed\n");
      exit(1);
    }
    status = pthread_mutex_unlock(&schedule_mutex);
    if (status != 0)
    {
      printf("Error: traffic_controller pthread_mutex_unlock failed\n");
      exit(1);
    }
    sleep(4); // Allow each direction to pass for TRUE minute
  }
  return NULL;
}

// Traffic of each directions
void *traffic(void *arg)
{
  int status;
  // Random seed
  srand(time(NULL));

  Direction direction = *(Direction *)arg;
  int group = direction / 2; // Index of each group of directions
  int i = 1;                 // Simulate vehicle number.
  while (TRUE)
  {

    status = pthread_mutex_lock(&schedule_mutex);
    if (status != 0)
    {
      printf("Error: traffic pthread_mutex_lock failed\n");
      exit(1);
    }
    while (current_direction != group)
    {
      // printf("%s waiting...\n", Direction_Names[direction]);
      status = pthread_cond_wait(&cond, &schedule_mutex);
      if (status != 0)
      {
        printf("Error: traffic pthread_cond_wait failed\n");
        exit(1);
      }
      i = 1;
    }
    status = pthread_mutex_unlock(&schedule_mutex);
    if (status != 0)
    {
      printf("Error: traffic pthread_mutex_unlock failed\n");
      exit(1);
    }

    status = pthread_mutex_lock(&mutexes[direction]);
    if (status != 0)
    {
      printf("Error: traffic pthread_mutex_lock 2 failed\n");
      exit(1);
    }
    printf("%d:%s direction is passing No.%d.\n", direction, Direction_Names[direction], i);
    usleep((float)rand() / RAND_MAX * 1000000 * 2); // Simulate passing time, Convert seconds to microseconds
    status = pthread_mutex_unlock(&mutexes[direction]);
    if (status != 0)
    {
      printf("Error: traffic pthread_mutex_unlock failed\n");
      exit(1);
    }
    usleep(10000); // Simulate traffic interval
    i++;
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  int status;
  pthread_t threads[DIRECTION_COUNT + 1];
  Direction directions[DIRECTION_COUNT];

  // Initialize mutexes and condition variable
  for (int i = 0; i < DIRECTION_COUNT; i++)
  {
    status = pthread_mutex_init(&mutexes[i], NULL);
    if (status != 0)
    {
      printf("Error: pthread_mutex_init %d failed\n", i);
      exit(1);
    }
    directions[i] = (Direction)i;
  }
  status = pthread_mutex_init(&schedule_mutex, NULL);
  if (status != 0)
  {
    printf("Error: pthread_mutex_init failed\n");
    exit(1);
  }
  status = pthread_cond_init(&cond, NULL);
  if (status != 0)
  {
    printf("Error: pthread_cond_init failed\n");
    exit(1);
  }
  // Create traffic controller thread, the last array of threads as the traffic controller thread.
  status = pthread_create(&threads[DIRECTION_COUNT], NULL, cross_traffic_controller, NULL);
  if (status != 0)
  {
    printf("Error: pthread_create cross_traffic_controller failed\n");
    exit(1);
  }
  // Create traffic threads
  for (int i = 0; i < DIRECTION_COUNT; i++)
  {
    status = pthread_create(&threads[i], NULL, traffic, &directions[i]);
    if (status != 0)
    {
      printf("Error: pthread_create %d failed\n", i);
      exit(1);
    }
  }
  // Wait for threads to complete
  for (int i = 0; i <= DIRECTION_COUNT; i++)
  {
    status = pthread_join(threads[i], NULL);
    if (status != 0)
    {
      printf("Error: pthread_join %d failed\n", i);
      exit(1);
    }
  }

  // Destroy mutexes and condition variable
  for (int i = 0; i < DIRECTION_COUNT; i++)
  {
    status = pthread_mutex_destroy(&mutexes[i]);
    if (status != 0)
    {
      printf("Error: pthread_mutex_destroy %d failed\n", i);
      exit(1);
    }
  }
  status = pthread_mutex_destroy(&schedule_mutex);
  if (status != 0)
  {
    printf("Error: pthread_mutex_destroy schedule_mutex failed\n");
    exit(1);
  }
  status = pthread_cond_destroy(&cond);
  if (status != 0)
  {
    printf("Error: pthread_cond_destroy failed\n");
    exit(1);
  }

  return 0;
}
