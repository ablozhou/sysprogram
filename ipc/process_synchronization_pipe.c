/*
 * Example of synchronization using a pipe.
 * Author: Andy Zhou
 *
 * Imagine a parent process P creates two child processes C1 and C2.
 * Each child process must complete two tasks T1 and T2.
 * After C1 and C2 have both completed T1, the parent must print:
 * "All children have completed their first task."
 * After C1 and C2 have completed T2 and called exit, the parent must print:
 * "All children have completed their second task as well."
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    // for pipe, sleep, fork
#include <sys/types.h> //for p_id
#include <sys/wait.h>  //for wait

#define BUFSIZE 1024
// #define CHILDREN_SIZE 2

void doTask(int childId, char *taskName)
{
  printf("Child %d finished task %s.\n", childId, taskName);
}

// void *sysnchronization(void *data)
// {
//   int status;
//   int nRead, nWrite;
//   pid_t pid[CHILDREN_SIZE];
//   int pipeDescriptor[2];

//   int value;
//   status = pipe(pipeDescriptor);
//   // check status
//   if (status != 0)
//   {
//     printf("create pipe failed.\n");
//     exit(1);
//   }
// }

void main(int argc, char *argv[])
{
  int status;
  int nRead, nWrite;
  int pipeDescriptor[2];

  int value;
  if (argc < 2)
  {
    printf("Usage: %s numChild\n", argv[0]);
    exit(1);
  }
  int numChild = atoi(argv[1]);
  if (numChild < 0)
  {
    printf("child number must be positive\n");
    exit(1);
  }

  pid_t *pid[numChild] = malloc(numChild * sizeof(pid_t));

  status = pipe(pipeDescriptor);
  // check status
  if (status != 0)
  {
    printf("create pipe failed.\n");
    exit(1);
  }
  for (int i = 0; i < numChild; i++)
  {
    pid[i] = fork();
    switch (pid[i])
    {
    case -1:
      printf("fork child failed");
      exit(1);
    case 0:
      // child
      // close read pipe
      status = close(pipeDescriptor[0]);
      if (status != 0)
      {
        printf("close pipe read failed.\n");
        _exit(1);
      }
      doTask(i, "child1task");
      // write the pipe
      value = 1;
      nWrite = write(pipeDescriptor[1], &value, sizeof(int));
      if (nWrite != sizeof(int))
      {
        printf("write failed.\n");
        _exit(1);
      }
      status = close(pipeDescriptor[1]);
      if (status != 0)
      {
        printf("close pipe write failed.\n");
        _exit(1);
      }
      _exit(0);
    default:
      // parent
      // close write pipe descriptor
      status = close(pipeDescriptor[1]);
      if (status != 0)
      {
        printf("parent close write pipe failed.\n");
        exit(1);
      }
      nRead = read(pipeDescriptor[0], &value, sizeof(int));
      if (nRead != sizeof(int))
      {
        printf("Parent read from child failed.\n");
        exit(1);
      }
      printf("Parent read from child:%d", value);
    }
  }
  for (int i = 0; i < CHILDREN_SIZE; i++)
  {
    status = wait(pid[i]);
    if (status != 0)
    {
      printf("wait child failed.\n");
      exit(1);
    }
  }
  exit(0);
}