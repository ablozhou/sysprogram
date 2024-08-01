#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  int valuesToWrite[] = {1, 3, 5, 7, 9};
  int fifoDescriptor, status, size;
  int numWritten;
  if (argc < 2)
  {
    printf("Usage: %s fifoname.\n", argv[0]);
    exit(1);
  }
  fifoDescriptor = open(argv[1], O_WRONLY); // cannot use O_CREAT
  if (fifoDescriptor == -1)
  {
    printf("Failed to open the FIFO.\n");
    exit(EXIT_FAILURE);
  }

  size = 5 * sizeof(int);
  numWritten = write(fifoDescriptor, valuesToWrite, size);
  if (numWritten < size)
  {
    printf("Partial write error.\n");
    exit(EXIT_FAILURE);
  }

  status = close(fifoDescriptor);
  if (status == -1)
  {
    printf("Failed to close the FIFO.\n");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}