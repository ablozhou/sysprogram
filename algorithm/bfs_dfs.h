#ifndef BFS_DFS_H
#define BFS_DFS_H

#include "graph.h"

void initialize_search(graph *g);
void bfs(graph *g, int start);
int edge_classification(int x, int y);
void dfs(graph *g, int v);
void find_path(int start, int end, int parents[]);

#endif