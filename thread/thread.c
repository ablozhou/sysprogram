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
typedef struct tparam {
  unsigned int id;
  unsigned int num;
} tparam;

unsigned int count;
pthread_mutex_t mutex;
void * incrementCount(void *data)
{
  tparam * param = (tparam *) data;
  for (int i = 0; i < param->num; i++) {
    pthread_mutex_lock(&mutex);
    count++;
    pthread_mutex_unlock(&mutex);
    //printf("Thread: %u, new value of count: %u\n", param->id, count);
    printf("Thread1: %d, new value of count: %u\n", gettid(), count);
    //printf("Thread2: %lu, new value of count: %u\n",  pthread_self(), count);
  }
}

void main(int argc, char *argv[]) {
  int status;
  if (argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1],"--help") == 0) {
    printf("Usage: %s num.\n", argv[0]);
    exit(1);
  }

  int num = atoi(argv[1]);
  pthread_t t1, t2;
  tparam p1, p2;
  p1.num = num;
  p1.id = 1;
  p2.num = num;
  p2.id = 2;

  status = pthread_create(&t1, NULL, incrementCount, &p1);
  if (status != 0) {
    printf("create thread %d failed.\n", p1.id);
    exit(1);
  }
  status = pthread_create(&t2, NULL, incrementCount, &p2);
  if (status != 0) {
    printf("create thread %d failed.\n", p2.id);
    exit(1);
  }
  status = pthread_join(t1, NULL);
  if (status != 0) {
    printf("join thread %d failed.\n", p1.id);
    exit(1);
  }
  status = pthread_join(t2, NULL);
  if (status != 0) {
    printf("join thread %d failed.\n", p2.id);
    exit(1);
  }
  exit(0);
}