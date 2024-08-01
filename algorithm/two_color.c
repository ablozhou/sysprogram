#include <stdio.h>
#include <stdbool.h>
#include "graph.h"

#define UNCOLORED 0
#define WHITE 0
#define BLACK 1

int *color;
bool *discovered;

void twocolor(graph *g)
{
    int i; /*counter*/
    for (i = 1; i <= (g->nvertices); i++)
        color[i] = UNCOLORED;
    bool bipartite = true;
    initialize_search(g);
    for (i = 1; i <= (g->nvertices); i++)
    {
        if (!discovered[i])
        {
            color[i] = WHITE;
            bfs(g, i);
        }
    }
}

void process_edge(int x, int y)
{
    if (color[x] == color[y])
    {
        bool bipartite = false;
        printf("Warning: not bipartite, due to (%d, %d)\n", x, y);
    }
    color[y] = complement(color[x]);
}
int complement(int color)
{
    if (color == WHITE)
    {
        return (BLACK);
    }
    if (color == BLACK)
    {
        return (WHITE);
    }
    return (UNCOLORED);
}
