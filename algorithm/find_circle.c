#include <stdio.h>
#include <stdbool.h>

int parent[];
bool finished = false;
void process_edge(int x, int y)
{
    if (parent[y] != x)
    { // found back edge
        printf("Cycle from %d to %d\n", y, x);
        find_path(y, x, parent);
        finished = true;
    }
}