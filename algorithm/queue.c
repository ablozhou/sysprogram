#include <stdio.h>
#include <stdbool.h>

#include "queue.h"
// queue begin
void init_queue(queue *q)
{
    q->first = 0;
    q->last = QUEUESIZE - 1;
    q->count = 0;
}

void enqueue(queue *q, item_type x)
{
    if (q->count >= QUEUESIZE)
    {
        printf("Warning: queue overflow enqueue x=%d\n", x);
    }
    else
    {
        q->last = (q->last + 1) % QUEUESIZE;
        q->q[q->last] = x;
        q->count = q->count + 1;
    }
}

item_type dequeue(queue *q)
{
    item_type x;

    if (q->count <= 0)
    {
        printf("Warning: empty queue dequeue.\n");
    }
    else
    {
        x = q->q[q->first];
        q->first = (q->first + 1) % QUEUESIZE;
        q->count = q->count - 1;
    }
    return (x);
}

item_type headq(queue *q)
{
    return (q->q[q->first]);
}

int empty_queue(queue *q)
{
    if (q->count <= 0)
    {
        return (TRUE);
    }
    return (FALSE);
}

void print_queue(queue *q)
{
    int i, j;

    i = q->first;

    while (i != q->last)
    {
        printf("%d ", q->q[i]);
        i = (i + 1) % QUEUESIZE;
    }

    printf("%2d ", q->q[i]);
    printf("\n");
}
// queue end