/*
1. Let’s take a closer look at what happens when there is a context switch without mutual exclusion. Use
the code sample POSIX Threads and Mutex on Webcampus as your starting point. Save this code as
thread.c and make the following changes.
(a) Update the function incrementCount so that its input is a pointer to a structure with two fields:
• a unique (unsigned integer) identifier set by the main thread, and
• the number of times to increment count.
(b) Comment out all code that initializes, locks, and unlocks the mutex.
(c) Whenever incrementCount increments count, it should print out the following line to screen:
Thread: <this thread’s identifier>, new value of count: <value>
(d) Compile the program using cc thread.c -o thread -pthread -Werror and run it using
./thread 100000 > output.txt
so that the output is written to the file output.txt instead of the terminal.
(e) Examine the file output.txt using your text editor and try to identify those pairs of lines in the
output file that indicate a context switch. For example, suppose the main thread had created two
child threads and assigned them identifiers 10 and 20. If the first line in the output file starts
with “Thread: 10” then search for “Thread: 20” and then search for “Thread: 10” again. Did
count get updated correctly?
(f) Now uncomment the lines that initialize, lock, and unlock the mutex and recompile the code, run
it again as above, and rexamine whether count had the correct value after context switches.
2. Modify the implementations in Problem 1 such that each child thread calls gettid (see: man 2 gettid)
and uses the result as the thread identifier while printing
Thread: <this thread’s identifier>, new value of count: <value>
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

volatile int count = 0;
typedef struct Context
{
  unsigned int id;
  int count;
} Context;
void *incrementCount(void *data)
{
  int status;
  Context *context = (Context *)data;
  printf("context:%u, %d\n", context->id, context->count);
  for (int i = 0; i < context->count; i++)
  {

    status = pthread_mutex_lock(&mutex);
    if (status != 0)
    {
      printf("lock failed %u.\n", context->id);
      exit(1);
    }

    count++;
    printf("Thread: %d, new value of count: %d\n", context->id, count);
    status = pthread_mutex_unlock(&mutex);
    if (status != 0)
    {
      printf("unlock failed %u.\n", context->id);
      exit(1);
    }
  }
  return NULL;
}

void main(int argc, char *argv[])
{
  int status;
  if (argc < 2)
  {
    printf("Usage: %s count.\n", argv[0]);
    exit(1);
  }

  Context context1, context2;
  context1.id = 1;
  context2.id = 2;
  context1.count = atoi(argv[1]);
  context2.count = atoi(argv[1]);

  pthread_t thread1, thread2;
  status = pthread_create(&thread1, NULL, incrementCount, &context1);
  if (status != 0)
  {
    printf("create thread %d failed\n", context1.id);
    exit(1);
  }
  status = pthread_create(&thread2, NULL, incrementCount, &context2);
  if (status != 0)
  {
    printf("create thread %d failed\n", context2.id);
    exit(1);
  }
  status = pthread_join(thread1, NULL);
  if (status != 0)
  {
    printf("join thread %d failed\n", context1.id);
    exit(1);
  }

  status = pthread_join(thread2, NULL);
  if (status != 0)
  {
    printf("join thread %d failed\n", context2.id);
    exit(1);
  }
  exit(0);
}
