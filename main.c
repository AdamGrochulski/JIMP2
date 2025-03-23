#include <stdio.h>
#include <stdlib.h>

/*FILE graf.csrrg
 * Linijki 1-3 potrzebne do macierzy
 * Linijki 2-5 potrzebne do grafu
 */


typedef struct node {
    int nodeID; // index of a node
    int arraySize;
    int divSize; // divSize = arraySize/2
    int *allEdges; // all connections with a node
    int *internalEdges; // all internal connections with a node
    int *externalEdges; // all external connections with a node
    int difference; // difference = numExternal - numInternal
}*node;


typedef struct graph {
    node currentNode;
    int numNodes; // number of nodes
    node *next; // next node
}*graph;


/*Allocating memory for arrays, assigning nodeID and arraySize*/
node createNode(int nodeID, int arraySize) {
    node newNode = malloc(sizeof(node));
    newNode->nodeID = nodeID;
    newNode->arraySize = arraySize;
    newNode->divSize = arraySize/2;
    newNode->allEdges = malloc(sizeof(int) * newNode->arraySize);
    newNode->internalEdges = malloc(sizeof(int) * newNode->divSize);
    newNode->externalEdges = malloc(sizeof(int) * newNode->divSize);
    return newNode;
}


int numElements(const char *fileName, int lineNumber) {
    FILE *file = fopen(fileName, "r");
    int currentLine = 1;
    int i = 0;
    int number;
    while (fscanf(file, "%d", &number) == 1) {
        if (currentLine == lineNumber) {
            i++;
        }
        if (fgetc(file) == '\n') {
            currentLine++;
        }
    }
    fclose(file);
    return i;
}

/*function reads the line and returns an array*/
int *readLine(const char *fileName, int lineNumber, int n) {
    FILE *file = fopen(fileName, "r");
    int currentLine = 1, i = 0, number;
    int *array = malloc(sizeof(int) * n);
    while (fscanf(file, "%d", &number) == 1) {
        if (currentLine == lineNumber) {
            array[i++] = number;
            if (i >= n) break;
        }
        if (fgetc(file) == '\n') {
            currentLine++;
        }
    }
    fclose(file);
    return array;
}

void addAllEdges() {
    
}

int main(int argc, char *argv[]) {
    const char *fileName = "graf.csrrg";
    int lineNumber = 4;
    int n = numElements(fileName, lineNumber);
    int *array = readLine(fileName, lineNumber, n);
    for (int i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
    free(array);
    return 0;
}
