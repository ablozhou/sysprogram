/*1. Implement a program in which a parent process writes a string of any length (up to a maximum) to a
pipe, and a child process reads that string from the pipe and prints out the length of the string. Test
this program out with the following strings:
(a) Hello World! The length should be 12.
(b) Linux Programming The length should be 17.

 2. Implement a program in which a parent process writes a string of any length (up to a maximum) to a
“request” pipe, and a child process reads the string from the pipe, converts it to uppercase, and sends
the result to the parent using a “response” pipe. Test this program out with the same test cases as
the previous problem.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>

#define TRUE 1
#define MAX_SIZE 1024

int fd_pipe[2] = {0, 0};
void cleanup(void)
{
  int status;
  printf("%d exiting, cleaning up...\n", getpid());
  if (fd_pipe[0] != 0)
  {
    status = close(fd_pipe[0]);
    if (status != 0)
    {
      printf("close read fifo failed.\n");
      _exit(1);
    }
    printf("%d read pipe closed.\n", getpid());
  }
  if (fd_pipe[1] != 0)
  {
    status = close(fd_pipe[1]);
    if (status != 0)
    {
      printf("close write fifo failed.\n");
      _exit(1);
    }
    printf("%d write pipe closed.\n", getpid());
  }
}
void signal_handler(int signum)
{
  exit(0);
}
void main(int argc, char *argv[])
{
  int status;

  int nRead = 0;
  int nWrite = 0;
  char buffer[MAX_SIZE];
  pid_t pid;

  status = atexit(cleanup);
  if (status == -1)
  {
    printf("Error register cleanup when exit.\n");
    exit(1);
  }

  if (signal(SIGINT, signal_handler) == SIG_ERR)
  {
    printf("Error signal handler.\n");
    exit(1);
  }
  status = pipe(fd_pipe);
  if (status == -1)
  {
    printf("Error opening pipe.\n");
    exit(1);
  }
  pid = fork();
  switch (pid)
  {
  case -1:
    printf("Error fork child.\n");
    exit(1);

  case 0:
    // child process
    printf("Child process %d created.\n", getpid());
    //  close write fd_pipe
    status = close(fd_pipe[1]);
    if (status == -1)
    {
      printf("Error child close write pipe.\n");
      exit(1);
    }
    fd_pipe[1] = 0;
    // Read from pipe
    while (TRUE)
    {
      while ((nRead = read(fd_pipe[0], buffer, MAX_SIZE - 1)) > 0)
      {
        buffer[nRead] = '\0';
        printf("Received string:%s \n", buffer);
        printf("%s The length should be %d\n", buffer, nRead);
      }
      if (nRead == -1)
      {
        printf("Error read from pipe.\n");
        exit(1);
      }
    }
    // never run to here
    status = close(fd_pipe[0]);
    if (status == -1)
    {
      printf("Error child close read pipe.\n");
      exit(1);
    }

    _exit(0);

  default:
    // parent process
    status = close(fd_pipe[0]);
    if (status == -1)
    {
      printf("Error child close read pipe.\n");
      exit(1);
    }
    fd_pipe[0] = 0;

    char *string[2] = {"Hello World!", "Linux Programming"};
    // for (int i = 0; i < sizeof(string); i++)
    while (TRUE)
    {
      printf("Input:\n");
      // size_t length;
      //  scanf("%s", buffer); scanf only get a word
      if (fgets(buffer, MAX_SIZE, stdin) == NULL)
      {
        printf("Error reading input.\n");
        exit(1);
      }
      nRead = strlen(buffer);
      if (nRead >= MAX_SIZE)
      {
        printf("Error string length exceeds maximum:%d.\n", MAX_SIZE);
        exit(1);
      }
      // remove \n
      if (nRead > 0)
      {
        if (buffer[nRead - 1] == '\n')
        {
          buffer[nRead - 1] = '\0';
        }
      }
      else
      {
        printf("input nothing.\n");
        continue;
      }

      nRead = strlen(buffer);
      // sprintf(buffer, "%s", string[i]);

      // Write to pipe
      nWrite = write(fd_pipe[1], buffer, nRead);
      if (nWrite != nRead)
      {
        printf("%s The length should be %d\n", buffer, nRead);
        exit(1);
      }
    }
    //  close write fd_pipe
    status = close(fd_pipe[1]);
    if (status == -1)
    {
      printf("Error child close write pipe.\n");
      exit(1);
    }
  }

  pid = waitpid(pid, &status, 0);
  printf("Child %d exited with status %d\n", pid, status);
  exit(0);
}
