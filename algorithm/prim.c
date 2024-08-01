#include <stdio.h>
#include <stdbool.h>
#include "graph.h"
#include <stdlib.h>

#include "bfs_dfs.h"

#define MAXINT 100007

int parent[MAXV + 1]; /* discovery relation */

/************************************************************/
/* [[[ prim_cut */
void prim(graph *g, int start)
{
    int i;                  /* counter */
    edgenode *p;            /* temporary pointer */
    bool intree[MAXV + 1];  /* is the vertex in the tree yet? */
    int distance[MAXV + 1]; /* cost of adding to tree */
    int v;                  /* current vertex to process */
    int w;                  /* candidate next vertex */
    int weight;             /* edge weight */
    int dist;               /* best current distance from start */

    for (i = 1; i <= g->nvertices; i++)
    {
        intree[i] = FALSE;
        distance[i] = MAXINT;
        parent[i] = -1;
    }

    distance[start] = 0;
    v = start;

    while (intree[v] == FALSE)
    {
        intree[v] = TRUE;
        p = g->edges[v];
        while (p != NULL)
        {
            w = p->y;
            weight = p->weight;
            if ((distance[w] > weight) && (intree[w] == FALSE))
            {
                distance[w] = weight;
                parent[w] = v;
            }
            p = p->next;
        }
        v = 1;
        dist = MAXINT;
        for (i = 1; i <= g->nvertices; i++)
        {
            if ((intree[i] == FALSE) && (dist > distance[i]))
            {
                dist = distance[i];
                v = i;
            }
        }
    }
}
/* ]]] */

int main(void)
{
    graph g;
    int i;

    read_graph(&g, FALSE);

    prim(&g, 1);

    printf("Out of Prim\n");

    for (i = 1; i <= g.nvertices; i++)
    {
        find_path(1, i, parent);
    }
    printf("\n");

    return 0;
}