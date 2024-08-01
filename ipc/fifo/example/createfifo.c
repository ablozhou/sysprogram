/* Create a new FIFO */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
  int status;
  if (argc < 2)
  {
    printf("Usage: %s fifoname.\n", argv[0]);
    exit(1);
  }
  status = mkfifo(
      argv[1],
      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH); /* S_IXUSR, S_IXGRP, S_IXOTHER should NEVER be here */

  if (status == -1)
  {
    printf("Failed to create the FIFO.\n");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}