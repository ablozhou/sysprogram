
/*
 * Parent process writes the message "Hello world" to the pipe.
 * Child process reads the message and prints it.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[])
{
  int pipeDescriptor[2];
  int status, childStatus;
  char message[100];
  char value;
  int childId;
  int index;
  int numWritten, numRead, numValues;
  int messageLength;

  strcpy(message, "Hello world");

  status = pipe(pipeDescriptor);
  if (status == -1)
  {
    printf("Failed to create pipe.\n");
    exit(EXIT_FAILURE);
  }

  /* At this point, pipeDescriptor[0] is the read descriptor */
  /* pipeDescriptor[1] is the write descriptor */

  childId = fork();
  switch (childId)
  {
  case -1:
    printf("Failed to create child process.\n");
    exit(EXIT_FAILURE);

  case 0:
    /* Close the write descriptor */
    status = close(pipeDescriptor[1]);
    if (status == -1)
    {
      printf("Child failed to close write descriptor.\n");
      _exit(EXIT_FAILURE);
    }

    numRead = read(pipeDescriptor[0], &value, sizeof(char));
    while (numRead > 0)
    {
      if (value == '\0')
      {
        printf("\n");
        break;
      }

      printf("%c", value);
      numRead = read(pipeDescriptor[0], &value, sizeof(char));
    }

    if (numRead == -1)
    {
      printf("Child failed to read from the pipe.\n");
      _exit(EXIT_FAILURE);
    }

    /* Close the read descriptor */
    status = close(pipeDescriptor[0]);
    if (status == -1)
    {
      printf("Child failed to close read descriptor.\n");
      _exit(EXIT_FAILURE);
    }

    _exit(EXIT_SUCCESS);

  default:
    /* Close the read descriptor */
    status = close(pipeDescriptor[0]);
    if (status == -1)
    {
      printf("Parent failed to close read descriptor.\n");
      exit(EXIT_FAILURE);
    }

    messageLength = strlen(message) + 1; /* including \0 */
    numWritten = write(pipeDescriptor[1], message, messageLength);
    if (numWritten != messageLength)
    {
      printf("Partial write error.\n");
      exit(EXIT_FAILURE);
    }

    /* Close the write descriptor */
    status = close(pipeDescriptor[1]);
    if (status == -1)
    {
      printf("Parent failed to close write descriptor.\n");
      exit(EXIT_FAILURE);
    }

    pid_t pid = wait(&childStatus);
    if (pid == -1)
    {
      printf("Failed to wait for child process to complete.\n");
      exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
  }
}