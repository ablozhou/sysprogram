#include <stdio.h>

#include "graph.h"
extern bool processed[];  /* which vertices have been processed */
extern bool discovered[]; /* which vertices have been found */
extern int parent[];      /* discovery relation */
int main(void)
{
    graph g;
    int i;

    read_graph(&g, FALSE);
    print_graph(&g);
    initialize_search(&g);
    bfs(&g, 1);

    for (i = 1; i <= g.nvertices; i++)
    {
        printf(" %d", parent[i]);
    }
    printf("\n");

    for (i = 1; i <= g.nvertices; i++)
    {
        find_path(1, i, parent);
    }
    printf("\n");

    return 0;
}