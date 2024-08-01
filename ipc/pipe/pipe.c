#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
// what's the difference between pipe and fifo
// pipe is anonymous queue. fifo is a named pipe, or named queue.

// TODO turn the child into write and parent read.
int main(int argc, char *argv[])
{

  // int pipe2(int pipefd[2], int flags);
  int pipeDescriptor[2];
  int status;
  pid_t childPid;
  pid_t id;
  int values[] = {100, 50, 75, 25};
  int values1[4];
  int index;
  int nRead, nWrite;
  int value;
  // NOTICE: create pipe first, if call fork to create child first, then the pipe will read nothing from the pipe.
  status = pipe(pipeDescriptor);
  if (status == -1)
  {
    printf("Failed to create pipe descriptor\n");
    exit(1);
  }
  childPid = fork();
  switch (childPid)
  {
  case -1:
    printf("Failed to create child process\n");
    exit(1);

  case 0:

    // child process
    // write to the pipe

    printf("parent id: %d, child pid: %d\n", getppid(), getpid());
    status = close(pipeDescriptor[0]);
    if (status == -1)
    {
      printf("Parent failed to close read descriptor\n");
      exit(1);
    }
    // for (index = 0; index < 4; index++)
    // {
    sleep(1);
    nWrite = write(pipeDescriptor[1], values, 4 * sizeof(int));
    if (nWrite < 4 * sizeof(int))
    {
      printf("Partial write error\n");
      exit(1);
    }
    printf("tid: %d, write:%d\n", getpid(), values[index]);
    //}

    // close the write descriptor
    status = close(pipeDescriptor[1]);
    if (status == -1)
    {
      printf("failed to close the write descriptor\n");
      exit(1);
    }
    _exit(0);
  default:
    // parent process
    // read  the pipe
    // close the read descriptor since I will no be
    // close the write pipe first.
    status = close(pipeDescriptor[1]);
    if (status == -1)
    {
      printf("failed close write ds\n");
      exit(1); // you don't want terminate the parent process, so add underscore, call _exit.
    }
    while ((nRead = read(pipeDescriptor[0], values1, 4 * sizeof(int))) > 0)
    {
      for (int i = 0; i < nRead / sizeof(int); i++)
      {
        printf("tid: %d, read:%d\n", getpid(), values1[i]);
      }

      sleep(1);
    }
    if (nRead == -1)
    {
      printf("Child failed to read pipe.\n");
      exit(1); // you don't want terminate the parent process, so add underscore, call _exit.
    }
    status = close(pipeDescriptor[0]);
    if (status == -1)
    {
      printf("Close read pipe descriptor failed.\n");
      exit(1);
    }
  }

  id = wait(&status);
  if (id == -1)
  {
    printf("Parent failed to wait for child\n");
    exit(1);
  }
  return 0;
}
