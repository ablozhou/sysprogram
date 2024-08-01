#include <stdio.h>

#include "bfs_dfs.h"

#include "queue.h"

/* [[[ graph_arrays_cut */
bool processed[MAXV+1];     /* which vertices have been processed */
bool discovered[MAXV+1];    /* which vertices have been found */
int parent[MAXV+1];         /* discovery relation */
/* ]]] */

int entry_time[MAXV+1];     /* time of vertex entry */
int exit_time[MAXV+1];      /* time of vertex exit */
int time;                   /* current event time */

bool finished = FALSE;      /* if true, cut off search immediately */

/* [[[ init_search_cut */
void initialize_search(graph *g) {
    int i;                  /* counter */

    time = 0;

    for (i = 0; i <= g->nvertices; i++) {
        processed[i] = FALSE;
        discovered[i] = FALSE;
        parent[i] = -1;
    }
}
/* ]]] */

/* [[[ bfs_cut */
void bfs(graph *g, int start) {
    queue q;            /* queue of vertices to visit */
    int v;              /* current vertex */
    int y;              /* successor vertex */
    edgenode *p;        /* temporary pointer */

    init_queue(&q);
    enqueue(&q, start);
    discovered[start] = TRUE;

    while (empty_queue(&q) == FALSE) {
        v = dequeue(&q);
        process_vertex_early(v);
        processed[v] = TRUE;
        p = g->edges[v];
        while (p != NULL) {
            y = p->y;
            if ((processed[y] == FALSE) || g->directed) {
                process_edge(v, y);
            }
            if (discovered[y] == FALSE) {
                enqueue(&q,y);
                discovered[y] = TRUE;
                parent[y] = v;
		    }
            p = p->next;
        }
        process_vertex_late(v);
    }
}
/* ]]] */

/* [[[ eclass_cut */
int edge_classification(int x, int y) {
    if (parent[y] == x) {
        return(TREE);
    }

    if (discovered[y] && !processed[y]) {
        return(BACK);
    }

    if (processed[y] && (entry_time[y]>entry_time[x])) {
        return(FORWARD);
    }

    if (processed[y] && (entry_time[y]<entry_time[x])) {
        return(CROSS);
    }

    printf("Warning: self loop (%d,%d)\n", x, y);

    return -1;
}
/* ]]] */

/* [[[ dfs_cut */
void dfs(graph *g, int v) {
    edgenode *p;        /* temporary pointer */
    int y;              /* successor vertex */

    if (finished == TRUE) {
        return;         /* allow for search termination */
    }

    discovered[v] = TRUE;
    time = time + 1;
    entry_time[v] = time;

    process_vertex_early(v);

    p = g->edges[v];
    while (p != NULL) {
        y = p->y;
        if (discovered[y] == FALSE) {
            parent[y] = v;
            process_edge(v, y);
            dfs(g, y);
        } else if ((!processed[y]) || (g->directed)) {
            process_edge(v, y);
        }

        if (finished == TRUE) {
            return;
        }

        p = p->next;
    }

    process_vertex_late(v);

    time = time + 1;
    exit_time[v] = time;

    processed[v] = TRUE;
}
/* ]]] */

/* [[[ find_path_cut */
void find_path(int start, int end, int parents[]) {
    if ((start == end) || (end == -1)) {
        printf("\n%d", start);
    } else {
        find_path(start, parents[end], parents);
        printf(" %d", end);
    }
}
/* ]]] */