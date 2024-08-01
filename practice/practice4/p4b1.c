/*
  CSCI-7645: Practice Problem Set 4b
Fairleigh Dickinson University Vancouver
Summer 2024
Problems
1. Suppose a parent process creates n child processes, and each child must complete m tasks, namely,
T1, T2, ..., Tm, where n and m are positive integers provided by the user through command line arguments.
The parent process wants to find out when all the child processes have completed T1, when all the child processes have completed T2, and so on, and when all the children have completed all their tasks.
Implement the above using an array of m − 1 pipes.
2. Implement the above using a only one pipe.
3. Implement a program in which two unrelated processes, a writer and a reader, communicate using a
FIFO. The writer creates a FIFO and writes a string of any length (up to a maximum) to the FIFO,
whereas the reader reads that string from the FIFO and prints out the length of the string after which
it deletes the FIFO. Test this program out with the following strings:
(a) Hello World! The length should be 12.
(b) Linux Programming The length should be 17.
4. Implement a program in which two unrelated processes, P1 and P2 communicated using two FIFOs.
P2 creates a “request” FIFO whereas P1 creates a “response” FIFO. P1 writes a string of any length
(up to a maximum) to the request FIFO. P2 reads the string from the request FIFO, converts it to
uppercase, and sends the result to P1 using the response FIFO. P1 then prints the string received as
response. Using exit handlers, each process deletes the FIFO it created. Test this program out with a
variety of sample inputs.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

#define TRUE 1
#define MAX_SIZE 1024
void cleanup(void)
{
  printf("clearn up %d.\n", getpid());
}

// Handle Ctrl+c for exit and clean
void signal_handler(int signal)
{
  exit(0);
}

void child_process(int id, int (*fdPipes)[2], int mTasks)
{
  int status;
  size_t nWrite;
  int i = id;
  char buffer[MAX_SIZE];
  printf("Child %d forked,pid:%d\n", i, getpid());

  for (int j = 0; j < mTasks; j++)
  {
    // close read fdPipes
    status = close(fdPipes[j][0]);
    if (status == -1)
    {
      printf("Child %d:Error close read pipe %d, , pid:%d.\n", i, j, getpid());
      exit(1);
    }

    sleep(1);
    printf("Child %d: Task %d finished, pid:%d, sending to parent msg.\n", i, j, getpid());

    sprintf(buffer, "Child %d completed task %d.\n", i, j);

    printf("%s", buffer);
    nWrite = write(fdPipes[j][1], buffer, strlen(buffer));
    if (nWrite != strlen(buffer))
    {
      printf("Child %d: Error write pipe %d, pid:%d.\n", i, j, getpid());
      exit(1);
    }
    // when to close the pipe? 1 read multiple write?
    // 如果一个写端关闭了，只要仍有其他的写端存在，并且读端仍然打开，其他的写进程可以继续写数据而不会受到影响。关闭一个写端并不会影响其他写端继续向管道写数据。但是，如果所有写端都关闭了，那么管道的读端在读取时会返回EOF。
    status = close(fdPipes[j][1]);
    if (status == -1)
    {
      printf("Child %d:Error close write pipe %d, , pid:%d.\n", i, j, getpid());
      exit(1);
    }
    printf("Child %d: close write pipe %d.\n", i, j);
  }
  exit(0);
}

void main(int argc, char *argv[])
{
  int status;
  int nChildren, mTasks;
  int nRead;
  char buffer[1024];
  pid_t *pids;
  // fd pointers point to int[2], not int *fdPipes[2], which means a pointer array has 2 pointer elements.
  int(*fdPipes)[2];

  if (argc < 3 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
  {
    printf("Usage: %s nChildren mTasks\n", argv[0]);
    exit(1);
  }
  nChildren = (unsigned int)atoi(argv[1]);
  mTasks = (unsigned int)atoi(argv[2]);
  if (nChildren <= 0 || mTasks <= 0)
  {
    printf("nChildren and mTasks must be positive.\n");
    exit(1);
  }

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

  // malloc check position
  int **completed_child_tasks = malloc(nChildren * sizeof(int *));
  for (int i = 0; i < nChildren; i++)
  {
    completed_child_tasks[i] = calloc(mTasks, sizeof(int));
  }

  int *complete_tasks = calloc(mTasks, sizeof(int));

  // malloc pids and fds
  pids = malloc(nChildren * sizeof(pid_t));
  if (pids == NULL)
  {
    printf("Error malloc nChildren.\n");
    exit(1);
  }

  fdPipes = malloc(mTasks * sizeof(int[2]));
  if (fdPipes == NULL)
  {
    printf("Error malloc fdPipes.\n");
    exit(1);
  }
  // open m pipes.
  for (int i = 0; i < mTasks; i++)
  {
    status = pipe(fdPipes[i]);
    if (status == -1)
    {
      printf("Error open pipe %d.\n", i);
      exit(1);
    }
  }

  for (int i = 0; i < nChildren; i++)
  {
    pids[i] = fork();
    switch (pids[i])
    {
    case -1:
      printf("Error fork.\n");
      exit(1);
    case 0:
      child_process(i, fdPipes, mTasks);
    default:
      break;
    }
  }

  printf("parent pid:%d\n", getpid());
  for (int j = 0; j < mTasks; j++)
  {
    // close write fdPipes
    status = close(fdPipes[j][1]);
    if (status == -1)
    {
      printf("Error parent close write pipe %d.\n", j);
      exit(1);
    }
  }
  // loop to check and wait all tasks of children be finished.
  while (TRUE)
  {
    for (int j = 0; j < mTasks; j++)
    {
      // close write fdPipes
      // status = close(fdPipes[j][1]);
      // if (status == -1)
      // {
      //   printf("Error parent close write pipe %d.\n", j);
      //   exit(1);
      // }

      // if (complete_tasks[j] == 1)
      // {
      //   printf("task %d finished, continue.\n", j);
      //   continue;
      // }
      printf("Parent reading task %d\n", j);
      nRead = read(fdPipes[j][0], buffer, MAX_SIZE - 1);
      if (nRead < 0)
      {
        printf("Error parent read pipe %d, pid: %d.\n", j, getpid());
        exit(1);
      }
      else if (nRead == 0)
      {
        printf("parent read pipe %d EOF, continue.\n", j);
        continue;
      }
      buffer[nRead] = '\0';
      printf("Parent received pipe message: %s.\n", buffer);

      // WARNING read may get 2 concat sentences.
      char delim[] = "\n";

      int nChildId = -1, nTaskId = -1;

      // split more than 1 message.
      char *s = strtok(buffer, delim);
      while (s != NULL)
      {
        sscanf(buffer, "Child %d completed task %d.", &nChildId, &nTaskId);
        printf("parent sscanf:Child %d complet task %d.\n", nChildId, nTaskId);
        completed_child_tasks[nChildId][nTaskId] = 1;

        // check all task n of each child finished or not
        int complete_task_n = 1;
        for (int i = 0; i < nChildren; i++)
        {
          if (completed_child_tasks[i][nTaskId] == 0)
          {
            complete_task_n = 0;
            break;
          }
        }
        if (complete_task_n == 1)
        {
          complete_tasks[nTaskId] = 1;
          printf("Task %d finished.\n", nTaskId);
        }
        s = strtok(NULL, delim);
      }
    }
    ///
    int all_tasks_complete = 1;
    for (int i = 0; i < mTasks; i++)
    {
      if (complete_tasks[i] == 0)
      {
        all_tasks_complete = 0;
        break;
      }
    }
    if (all_tasks_complete == 1)
    {
      // completed all tasks.
      printf("All tasks finished.\n");
      break;
    }
    sleep(1);
  }

  for (int i = 0; i < nChildren; i++)
  {
    // special wait parameter
    pid_t pid = wait(&status);
    if (pid == -1)
    {
      printf("Error wait %d.\n", pids[i]);
      exit(1);
    }
    printf("waiting %d closed.\n", pid);
  }

  for (int j = 0; j < mTasks; j++)
  {
    printf("close %d reading pipe.\n", j);
    status = close(fdPipes[j][0]);
    if (status == -1)
    {
      printf("Error parent close reading pipe %d, , pid:%d.\n", j, getpid());
      exit(1);
    }
  }
  free(pids);
  free(fdPipes);
  for (int i = 0; i < nChildren; i++)
  {
    free(completed_child_tasks[i]);
  }
  free(completed_child_tasks);
  free(complete_tasks);
}