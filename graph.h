#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int arraySize; // Number of elements in allEdges array
    int *allEdges; // all connections with a node
    int group;
    int divSize; // divSize = arraySize/2
    int *internalEdges; // all internal connections with a node
    int *externalEdges; // all external connections with a node
    int difference; // difference = numExternal - numInternal
}*node;

typedef struct graph {
    int nodeID; // index of a node
    node currentNode;
    int numNodes; // number of nodes
    struct graph *next; // next node
}*graph;

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

#endif