#ifndef KERNIGHANLIN_H
#define KERNIGHANLIN_H
#include "graph.h"
#include "Dijkstra.h"

typedef struct pair {
    int id;
    int nodeIndexA;
    int nodeIndexB;
    int gain;
    int *externalArrayNodeA; // Zawiera wierzchołki zewnętrzne dla najlepszej możliwej grupy do podziału (dla B)
    int externalArrayNodeASize;
    int *externalArrayNodeB; // Zawiera wierzchołki dla najlepszej możliwej grupy do podziału (dla A)
    int externalArrayNodeBSize;
    int DifferenceA;
    int DifferenceB;
    struct pair* next;
}*pair;

graph locateNode(graph Graph, int index);
int findBestGroup(graph Graph, graph graphOrigin);
int getExternalArraySize(graph Graph, graph graphOrigin);
void assignExternalArrayNodeA(graph Graph, pair Pair, graph graphOrigin);
int countDifference(graph gB, graph gA, graph graphOrigin);
int countGain(pair Pair);
void adjustNodeB(graph Graph, pair Pair, graph graphOrigin);
pair initializePair();
pair createPairs(graph graphOriginal);
void freePair(pair Pair);
void KernighanLinAlgorithm(graph graphOriginal);

#endif
