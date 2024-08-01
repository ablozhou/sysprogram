/*In this project, we will develop a traffic-control system, using mutexes, condition variables, FIFO and semaphores in the Linux OS system to simulate it.
The intersection traffic system using mutex and Condition variables,
a single-lane bridge traffic control using FIFO and mutex, and
a parking lot using semophore, mutex and condition variables.
This simulator traffic control system is programmed in C and tested on Ubuntu using
multi-thread or multi-process techniques to help understanding the mechanisms in
concurrent scenarios when access sharing resources.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> // for mode
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>    // for file control flags
#include <sys/wait.h> //for wait child process

#define PARKING_LOT "/sem_parking"
#define VEHICLE "/sem_vehicle"

#define TRUE 1

// Define two semaphone to wait for full or empty of the parking lot
// Because the semaphone just wait when the semaphone is 0, but not block when the semaphone is at maximum.
// So we have to implement my own semaphone by two semaphores, or using pthread condition variables and mutexs to block
typedef struct
{
  sem_t *parking; // post increase space, wait decrease space
  sem_t *vehicle; // post increase vehicle, wait decrease vehicle
} mysem_t;

// Semaphore to manage parking lot spaces.
mysem_t *mysem = NULL;

// init_space: initial spaces, capacity:max spaces
mysem_t *mysem_open(int init_space, int capacity)
{
  sem_t *sem = sem_open(PARKING_LOT, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH, init_space);
  // printf("opening semaphore: %ld\n", (long)sem);
  if (sem == NULL || sem == SEM_FAILED)
  {
    printf("Error opening semaphore: %ld\n", (long)sem);
    return NULL;
  }
  sem_t *semv = sem_open(VEHICLE, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH, capacity - init_space);
  // printf("opening semaphore: %ld\n", (long)sem);
  if (semv == NULL || semv == SEM_FAILED)
  {
    printf("Error opening semaphore: %ld\n", (long)semv);
    return NULL;
  }

  mysem = malloc(sizeof(mysem_t));
  if (mysem == NULL)
  {
    return NULL;
  }
  mysem->parking = sem;
  mysem->vehicle = semv;
  return mysem;
}

int mysem_close(mysem_t *mysem)
{
  if (mysem == NULL)
  {
    return 0;
  }
  if (sem_close(mysem->parking) == -1 || sem_close(mysem->vehicle) == -1)
  {
    return -1;
  }
  // release memory
  free(mysem);
  mysem = NULL;
  return 0;
}

int mysem_unlink()
{
  if (sem_unlink(PARKING_LOT) == -1 || sem_unlink(VEHICLE) == -1)
  {
    return -1;
  }
  return 0;
}

int mysem_getvalue(mysem_t *mysem, int *value)
{
  if (mysem == NULL)
  {
    return 0;
  }
  if (sem_getvalue(mysem->parking, value) == -1)
  {
    return -1;
  }
  return 0;
}

// Wait when parking lot is full.
int mysem_wait(mysem_t *mysem)
{
  if (mysem == NULL)
  {
    return 0;
  }
  if (sem_wait(mysem->parking) == -1 || sem_post(mysem->vehicle) == -1)
  {
    return -1;
  }
  return 0;
}
// Wait when parking lot is empty.
int mysem_post(mysem_t *mysem)
{
  if (mysem == NULL)
  {
    return 0;
  }
  if (sem_wait(mysem->vehicle) == -1 || sem_post(mysem->parking) == -1)
  {
    return -1;
  }
  return 0;
}

void cleanup(void)
{
  int status;
  if (mysem != NULL)
  {
    status = mysem_close(mysem);
    if (status != 0)
    {
      printf("Error close mysem: %d.", status);
    }
  }
  status = mysem_unlink(mysem);
  if (status != 0)
  {
    printf("Error mysem_unlink %s, %s, %d", PARKING_LOT, VEHICLE, status);
  }
}

// Handle Ctrl+c for exit and clean
void signal_handler(int signal)
{
  exit(0);
}
void signal_handler_child(int signal)
{
  _exit(0);
}
void main(int argc, char *argv[])
{
  int status, result;

  int nextVehicle = 1;
  int entranceCount = 1;
  int exitCount = 1;
  int capacity = 10;
  int space = 0;
  pid_t *pid_ent;
  pid_t *pid_exit;
  int nRead;

  if (argc < 4 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
  {
    printf("Usage: %s capacity entrance_count exit_count\n", argv[0]);
    exit(1);
  }
  capacity = atoi(argv[1]);
  entranceCount = atoi(argv[2]);
  exitCount = atoi(argv[3]);

  if (entranceCount <= 0 || exitCount <= 0)
  {
    printf("entrance_count and exit_count must be positive\n");
    exit(1);
  }

  printf("Welcome to the parking lot in Vancouver city, there are %d entries and %d exits, %d spaces to parking.\n", entranceCount, exitCount, capacity);
  // Register clean up function
  status = atexit(cleanup);
  if (status != 0)
  {
    printf("Error register cleanup %d", status);
    exit(1);
  }

  // Remove existing semaphone
  status = mysem_unlink(mysem);
  if (status == -1)
  {
    printf("Error mysem_unlink: %d", status);
    // exit(1);
  }
  // Create a new semaphone
  mysem = mysem_open(capacity, capacity);

  if (mysem == NULL)
  {
    printf("Error opening semaphore.\n");
    exit(1);
  }

  status = mysem_getvalue(mysem, &space);
  if (status == -1)
  {
    printf("Error mysem_getvalue: %d", status);
    exit(1);
  }
  printf("mysem_open successful, capacity is %d.\n", space);

  // malloc
  pid_ent = (pid_t *)malloc(entranceCount * sizeof(pid_t));
  pid_exit = (pid_t *)malloc(exitCount * sizeof(pid_t));

  // fork entry process
  for (int i = 0; i < entranceCount; i++)
  {
    printf("Fork entry process %d\n", i);
    pid_ent[i] = fork();
    switch (pid_ent[i])
    {
    case -1:
      printf("Error fork entrance %d.\n", i);
      exit(1);
    case 0:
      // child process
      printf("Entry %d opened.\n", i);

      // Register CTRL-C signal handlers for child
      if (signal(SIGINT, signal_handler_child) == SIG_ERR)
      {
        printf("Error register signal.\n");
        _exit(1);
      }
      int vid = 0;

      while (TRUE)
      {
        status = mysem_getvalue(mysem, &space);
        if (status == -1)
        {
          printf("Error mysem_getvalue entry %d", i);
          _exit(1);
        }
        printf("Vehicle %d entering entrance %d, space left %d.\n", vid, i, space);
        status = mysem_wait(mysem);
        if (status == -1)
        {
          printf("Error mysem_wait entry %d, vehicle %d.\n", i, vid);
          _exit(1);
        }

        printf("Vehicle %d entered.\n", vid);
        sleep(1);
        vid++;
      }
      // will never run to here
      _exit(0);
    default:
      /* code */
      break;
    }
  }

  // fork exit process
  for (int i = 0; i < exitCount; i++)
  {
    printf("Fork exit process %d\n", i);
    pid_exit[i] = fork();
    switch (pid_exit[i])
    {
    case -1:
      printf("Error fork exit %d.\n", i);
      _exit(1);
    case 0:
      // child process
      printf("Exit %d opened.\n", i);
      // Register CTRL-C signal handlers for child
      if (signal(SIGINT, signal_handler_child) == SIG_ERR)
      {
        printf("Error register signal.\n");
        _exit(1);
      }
      int vid = 1;

      while (TRUE)
      {

        status = mysem_post(mysem);
        if (status == -1)
        {
          printf("Error mysem_post exit %d, vehicle %d.\n", i, vid);
          _exit(1);
        }
        status = mysem_getvalue(mysem, &space);
        if (status == -1)
        {
          printf("Error mysem_getvalue entry %d", i);
          _exit(1);
        }
        printf("Vehicle %d left from %d exit, space %d.\n", vid, i, space);
        sleep(2);
        vid++;
      }
      // never to here
      _exit(0);
    default:
      /* code */
      break;
    }
  }

  // Register CTRL-C signal handlers for parent process
  if (signal(SIGINT, signal_handler) == SIG_ERR)
  {
    printf("Error register signal.\n");
    exit(1);
  }
  // wait for child process exit
  for (int i = 0; i < entranceCount; i++)
  {
    waitpid(pid_ent[i], &result, 0);
    printf("Entrance %d closed.\n", i);
  }
  for (int i = 0; i < exitCount; i++)
  {
    waitpid(pid_exit[i], &result, 0);
    printf("Exit %d closed.\n", i);
  }

  free(pid_ent);
  free(pid_exit);
  printf("Parking lot closed.\n");
}