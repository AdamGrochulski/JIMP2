#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int arraySize; // Number of elements in allEdges array
    int *allEdges; // all connections with a node
    int group;
    int internalSize;
    int externalSize;
    int *internalEdges; // all internal connections with a node
    int *externalEdges; // all external connections with a node
    int difference; // difference = numExternal - numInternal (dla najlepszego externalSize)
    int locked; // 0 jeśli możliwa jest operacja zamiany, 1 jeśli nie można zamienić wierzchołka
}*node;

typedef struct graph {
    int nodeID; // index of a node
    node currentNode;
    int numNodes; // number of nodes
    struct graph *next; // next node
}*graph;

typedef struct checkedNodes {
    int* adjacentNodes;
}*checkedNodes;


node createNode(int arraySize);
node findNode(graph Graph, int nodeIndex);
int numElements(const char *fileName, int lineNumber);
int *readLine(const char *fileName, int lineNumber, int n);
int findLastNode(const char *fileName);
graph declareGraph(const char *fileName);
graph findNodeID(graph Miautoni, int nodeID);
graph createGraph(const char *fileName);
void printGraph(graph Graph);
void freeGraph(graph Graph);
void checkInputFile(char *fileName);

#endif