#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

#include "graph.h"

typedef struct paths {
    int value; //wartość ścieżki do wierzchołka ustalonego w iteracji kodu
    int nodeID; //indeks wierzchołka w głównej strukturze Graph
} paths;

int findMinValue(int pathsValues[],bool isNodeCounted[], int nodeCount);
bool evenOneFalse(bool isNodeCounted[], int nodeCount);
int compare(const void *a, const void *b);
void algDjikstry(graph Graph, paths *Path, int startingNode);
void createNodeGroups(graph Graph, int margin, int partition);
int findMax(graph Graph);
void subarraysize(graph Graph);
int findGroup(graph Graph, int node);
void subarray(graph Graph);

#endif