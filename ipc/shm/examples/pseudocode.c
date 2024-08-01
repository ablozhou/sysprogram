// PSEUDOCODE FOR PRODUCER CONSUMER PROBLEM
// Adapted from Modern Operating Systems by Andrew S. Tanenbaum

// Suppose that the buffer size is N and numItems denotes the number of items currently in the buffer.

// We need three semaphores, namely, mutex, numFull, and numEmpty.
// Their datatype is sem_t*

// INITIALIZATION

// Initialize numItems to 0 (since initially there are no items in the buffer)
// Initialize mutex to 1 (recall that 1 represents "unlocked" when a semaphore is used for mutual exclusion)
// Initialize numFull to 0 (since initially none of the slots in the buffer is full)
// Initialize numEmpty to N (since initially all the slots in the buffer are empty)

#define TRUE 1
int numItems = 0;
int numEmpty = 10;
int numFull = 0;
int mutex = 1;

// PRODUCER
void produce()
{
    while (TRUE)
    {
        sem_wait(numEmpty); /* Wait for at least one slot to be empty */
        /* Check for errors */

        sem_wait(mutex); /* Acquire lock */
        /* Check for errors */

        numItems++; /* Critical section */

        sem_post(mutex); /* Release lock */
        /* Check for errors */

        sem_post(numFull); /* Increment the number of slots that are full */
        /* Check for errors */
    }
}

// CONSUMER
void consume()
{
    while (TRUE)
    {
        sem_wait(numFull); /* Wait for at least one slot to become full */
        /* Check for errors */

        sem_wait(mutex); /* Acquire lock */
        /* Check for errors */

        numItems--; /* Critical section */

        sem_post(mutex); /* Release lock */
        /* Check for errors */

        sem_post(numEmpty); /* Increment the number of slots that are empty */
        /* Check for errors */
    }
}
