#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#define _GNU_SOURCE
#define _DECLARE_GETTID
#include <unistd.h>
#include <sys/types.h>

/*
thread.c:26:53: warning: implicit declaration of function ‘gettid’; did you mean ‘getgid’? [-Wimplicit-function-declaration]
My ldd version is (GNU libc) 2.28, and I get a compilation error in this case.
But I use another ldd (Ubuntu GLIBC 2.31-0ubuntu9.2) 2.31 is compiling fine.
This shows that compilation errors may occur in different GNU libc environments.
Would it be more appropriate to use syscall for better compatibility?
*/
typedef struct param_t
{
  unsigned int id;
  unsigned int num;
} param_t;

unsigned int count;
pthread_mutex_t mutex;
#define print_eno(eno, message, ...)                        \
  do                                                        \
  {                                                         \
    printf("Error: %d, " message "\n", eno, ##__VA_ARGS__); \
    exit(1);                                                \
  } while (0)
#define print_err(message, ...)           \
  do                                      \
  {                                       \
    printf("" message "", ##__VA_ARGS__); \
    exit(1);                              \
  } while (0)
#define SIZE 2

void *incrementCount(void *data)
{
  param_t *param = (param_t *)data;
  for (int i = 0; i < param->num; i++)
  {
    pthread_mutex_lock(&mutex);
    count++;
    pthread_mutex_unlock(&mutex);
    printf("Thread: %u, new value of count: %u\n", param->id, count);
    // printf("Thread1: %d, new value of count: %u\n", gettid(), count);
    // printf("Thread2: %lu, new value of count: %u\n", pthread_self(), count);
  }
}

void main(int argc, char *argv[])
{
  // print_error(1, "test");
  // print_error(2, "test %s", "test2");
  // print_error(3, "test %d", 3);
  int status;
  if (argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
  {
    print_err("Usage: %s num.\n", argv[0]);
  }

  int num = atoi(argv[1]);
  pthread_t thread[SIZE];

  param_t p[SIZE];
  for (int i = 0; i < SIZE; i++)
  {
    p[i].id = i;
    p[i].num = atoi(argv[1]);
    status = pthread_create(&thread[i], NULL, incrementCount, &p[i]);
    if (status != 0)
    {
      print_eno(status, "pthread_create %d failed.\n", p[i].id);
    }
    printf("pthread %d created.\n", p[i].id);
  }

  for (int i = 0; i < SIZE; i++)
  {
    status = pthread_join(thread[i], NULL);
    if (status != 0)
    {
      print_eno(status, "pthread_join %d failed.\n", p[i].id);
    }
    printf("pthread %d joined\n", p[i].id);
  }

  exit(0);
}