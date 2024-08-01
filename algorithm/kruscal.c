#include <stdio.h>
#include <stdlib.h>

#include "graph.h"

#define MAXINT 100007
#define SET_SIZE 1000

/* [[[ set_union_cut */
typedef struct
{
    int p[SET_SIZE + 1];    /* parent element */
    int size[SET_SIZE + 1]; /* number of elements in subtree i */
    int n;                  /* number of elements in set */
} set_union;

typedef struct
{
    int x, y;   /* adjacency info */
    int weight; /* edge weight, if any */
} edge_pair;

/************************************************************/
// set union begin
void set_union_init(set_union *s, int n)
{
    int i; /* counter */

    for (i = 1; i <= n; i++)
    {
        s->p[i] = i;
        s->size[i] = 1;
    }
    s->n = n;
}
/* ]]] */

/* [[[ set_union_find_cut */
int find(set_union *s, int x)
{
    if (s->p[x] == x)
    {
        return (x);
    }
    return (find(s, s->p[x]));
}
/* ]]] */

/* [[[ set_union_union_sets_cut */
void union_sets(set_union *s, int s1, int s2)
{
    int r1, r2; /* roots of sets */

    r1 = find(s, s1);
    r2 = find(s, s2);

    printf("s1=%d r1=%d s2=%d r2=%d\n", s1, r1, s2, r2);

    if (r1 == r2)
    {
        return; /* already in same set */
    }

    if (s->size[r1] >= s->size[r2])
    {
        s->size[r1] = s->size[r1] + s->size[r2];
        s->p[r2] = r1;
    }
    else
    {
        s->size[r2] = s->size[r1] + s->size[r2];
        s->p[r1] = r2;
    }
}
/* ]]] */

/* [[[ same_component_cut */
bool same_component(set_union *s, int s1, int s2)
{
    return (find(s, s1) == find(s, s2));
}
/* ]]] */

void print_set_union(set_union *s)
{
    int i; /* counter */

    for (i = 1; i <= s->n; i++)
    {
        printf("%i  set=%d size=%d \n", i, s->p[i], s->size[i]);
    }
    printf("\n");
}
// set union end

void to_edge_array(graph *g, edge_pair e[])
{
    int i, m;    /* counters */
    edgenode *p; /* temporary pointer */

    m = 0;
    for (i = 1; i <= g->nvertices; i++)
    {
        p = g->edges[i];
        while (p != NULL)
        {
            if (p->y > i)
            {
                e[m].x = i;
                e[m].y = p->y;
                e[m].weight = p->weight;
                m = m + 1;
            }
            p = p->next;
        }
    }
}

int weight_compare(const void *x, const void *y)
{
    edge_pair *_x = (edge_pair *)x;
    edge_pair *_y = (edge_pair *)y;

    if (_x->weight < _y->weight)
    {
        return (-1);
    }

    if (_x->weight > _y->weight)
    {
        return (1);
    }

    return (0);
}

/* [[[ kruskal_cut */
void kruskal(graph *g)
{
    int i;                 /* counter */
    set_union s;           /* set union data structure */
    edge_pair e[MAXV + 1]; /* array of edges data structure */

    set_union_init(&s, g->nvertices);

    printf("initialized set union\n");
    to_edge_array(g, e);
    qsort(&e, g->nedges, sizeof(edge_pair), &weight_compare);

    for (i = 0; i < (g->nedges); i++)
    {
        print_set_union(&s);
        if (!same_component(&s, e[i].x, e[i].y))
        {
            printf("edge (%d,%d) of weight %d in MST\n", e[i].x, e[i].y, e[i].weight);
            union_sets(&s, e[i].x, e[i].y);
        }
    }
}
/* ]]] */

int main(void)
{
    graph g;

    read_graph(&g, FALSE);

    print_graph(&g);

    kruskal(&g);

    printf("Out of Kruskal\n");

    return 0;
}