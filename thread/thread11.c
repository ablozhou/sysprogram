// cc threa1.c -o thread1 -Werror -pthread
// -Werror: warning to error
// -pthread: link code to pthread library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

volatile int count = 0; //shared resource
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //unlocked
// input means how many times changes.
void* threadFunc(void* input) {
  printf("I'am the child thread %d.\n", getthreadid());
  int status;
  int index;
  if (input == NULL)
  {
    printf("input cannot be NULL.\n");
    exit(1); // not pthread_exit()
  }
  int *pointerToInt = (int *)input; // go from void pointer to int pionter.
  int numIncrements = *pointerToInt; // get integer value.
  // equals to int numIncrements = *(int*) input //right to left convert
  for (index = 0; index < numIncrements; index++) {
    status = pthread_mutex_lock(&mutex); //acquire a lock
    if (status != 0) {
      printf("Failed to lock mutex.\n");
      exit(1);
    }

    count++; //critical section, when the for loop havn't completed in a short time, it has to go ready state.
    
    status = pthread_mutex_unlock(&mutex); //release mutex
    if (status != 0) {
      printf("Failed to unlock mutex.\n");
      exit(1);
    }
    //printf("%d\n", count);
  }

  pthread_exit(NULL); // finished it's work and complete.
}

int main(int argc, char* argv[]) {
  pthread_t thread1, thread2;
  int status = 0;
  void *result;
  int numIncrements = 1000000;
  printf("Parent is about to create child thread.\n");
  status = pthread_create(&thread1, NULL, threadFunc, (void*)&numIncrements);
  if(status != 0) {
    printf("Failed to create child thread: %d\n", status);
    exit(1);
  }
  status = pthread_create(&thread2, NULL, threadFunc, (void*)&numIncrements);
  if(status != 0) {
    printf("Failed to create child thread: %d\n", status);
    exit(1);
  }
  //wait or waitpid to wait the process to complete.
  // wait for the thread to avoid zombie thread.
  status = pthread_join(thread1, &result);
  if(status != 0) {
    printf("Failed to wait child thread:%d\n", status);
    exit(1);
  }
  status = pthread_join(thread2, &result);
  if(status != 0) {
    printf("Failed to wait child thread: %d\n", status);
    exit(1);
  }
  printf("Child thread has completed.\n");
  printf("final value:%d \n", count);
  exit(0);
}