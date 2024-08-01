#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  int numRead, fifoDescriptor, status, value;
  if (argc < 2)
  {
    printf("Usage: %s fifoname.\n", argv[0]);
    exit(1);
  }
  fifoDescriptor = open(argv[1], O_RDONLY);
  if (fifoDescriptor == -1)
  {
    printf("Failed to open FIFO for reading.\n");
    exit(EXIT_FAILURE);
  }

  numRead = read(fifoDescriptor, &value, sizeof(int));

  while (numRead > 0)
  {
    printf("%d\n", value);
    numRead = read(fifoDescriptor, &value, sizeof(int));
  }

  if (numRead == -1)
  {
    printf("Failed to read from FIFO.\n");
    exit(EXIT_FAILURE);
  }

  status = close(fifoDescriptor);
  if (status == -1)
  {
    printf("Failed to close the FIFO.\n");
    exit(EXIT_FAILURE);
  }

  // once have read the fifo, remove it.
  status = unlink(argv[1]);
  if (status == -1)
  {
    printf(" Failed to unlink the FIFO.\n");
    exit(1);
  }

  exit(EXIT_SUCCESS);
}