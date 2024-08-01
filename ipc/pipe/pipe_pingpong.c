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
  int pipePing[2];
  int pipePong[2];
  int status;
  pid_t childPid;
  pid_t id;
  // int values[] = {100, 50, 75, 25};
  int index;
  int nRead, nWrite;
  int value = 0;
  // NOTICE: create pipe first, if call fork to create child first, then the pipe will read nothing from the pipe.
  status = pipe(pipePing);
  if (status == -1)
  {
    printf("Failed to create pipe descriptor\n");
    exit(1);
  }
  status = pipe(pipePong);
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
    // read from the pipe

    // close the write pipe first.
    status = close(pipePing[1]);
    if (status == -1)
    {
      printf("failed close write ds\n");
      exit(1); // you don't want terminate the parent process, so add underscore, call _exit.
    }
    printf("parent id: %d, child pid: %d\n", getppid(), getpid());
    nWrite = write(pipePong[1], &value, sizeof(int));
    if (nWrite < sizeof(int))
    {
      printf("Partial write error\n");
      exit(1);
    }
    printf("tid: %d, send:%d\n", getpid(), value);

    while ((nRead = read(pipePing[0], &value, sizeof(int))) > 0)
    {
      printf("tid: %d, get:%d.\n ", getpid(), value);
      value += 1;
      sleep(1);
      nWrite = write(pipePong[1], &value, sizeof(int));
      if (nWrite < sizeof(int))
      {
        printf("Partial write error\n");
        exit(1);
      }
      printf("tid: %d, send:%d\n", getpid(), value);
    }
    if (nRead == -1)
    {
      printf("Child failed to read pipe.\n");
      _exit(1); // you don't want terminate the parent process, so add underscore, call _exit.
    }
    status = close(pipePing[0]);
    if (status == -1)
    {
      printf("Close read pipe descriptor failed.\n");
      exit(1);
    }
    status = close(pipePong[1]);
    if (status == -1)
    {
      printf("Close read pipe descriptor failed.\n");
      exit(1);
    }
    _exit(0);
  default:

    // parent process
    // write to the pipe
    // clse the read descriptor since I will no be

    status = close(pipePing[0]);
    if (status == -1)
    {
      printf("Parent failed to close read descriptor\n");
      exit(1);
    }

    while ((nRead = read(pipePong[0], &value, sizeof(int))) > 0)
    {
      sleep(1);
      printf("\t tid: %d, get:%d.\n ", getpid(), value);
      value += 1;
      nWrite = write(pipePing[1], &value, sizeof(int));
      if (nWrite < sizeof(int))
      {
        printf("Partial write error\n");
        exit(1);
      }
      printf("\t tid: %d, send:%d\n", getpid(), value);
    }
    // close the write descriptor
    status = close(pipePing[1]);
    if (status == -1)
    {
      printf("failed to close the write descriptor\n");
      exit(1);
    }
    _exit(0);
    //_exit(0);
  }

  id = wait(&status);
  if (id == -1)
  {
    printf("Parent failed to wait for child\n");
    exit(1);
  }
  return 0;
}
