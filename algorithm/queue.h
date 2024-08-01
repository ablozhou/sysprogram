#define QUEUESIZE 1000
#define TRUE 1
#define FALSE 0

typedef int item_type;
typedef struct
{
    int /*item_type*/ q[QUEUESIZE + 1]; /* body of queue */
    int first;                          /* position of first element */
    int last;                           /* position of last element */
    int count;                          /* number of queue elements */
} queue;

void init_queue(queue *q);
void enqueue(queue *q, item_type x);
item_type dequeue(queue *q);
item_type headq(queue *q);
int empty_queue(queue *q);
void print_queue(queue *q);