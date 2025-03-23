#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

/*FILE graf.csrrg
 * Linijki 1-3 potrzebne do macierzy
 * Linijki 2-5 potrzebne do grafu
 */


typedef struct node {
    int nodeID; // index of a node
    int arraySize;
    int divSize; // divSize = arraySize/2
    int *allEdges; // all connections with a node
    int group;
    int *internalEdges; // all internal connections with a node
    int *externalEdges; // all external connections with a node
    int difference; // difference = numExternal - numInternal
}*node;


typedef struct graph {
    node currentNode;
    int numNodes; // number of nodes
    struct graph *next; // next node
}*graph;


/*Allocating memory for arrays, assigning nodeID and arraySize*/
node createNode(int nodeID, int arraySize) {
    node newNode = malloc(sizeof(node));
    if(!newNode) {
        perror("Memory allocation failed!");
        exit(2137);
    }
    newNode->nodeID = nodeID;
    newNode->arraySize = arraySize;
    newNode->divSize = arraySize/2;
    newNode->allEdges = malloc(sizeof(int) * newNode->arraySize);
    newNode->internalEdges = malloc(sizeof(int) * newNode->divSize);
    newNode->externalEdges = malloc(sizeof(int) * newNode->divSize);
    return newNode;
}

/*Number of elements in a "*.csrrg" file*/
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
    if(!file) {
        perror("Error: you're screwed");
        exit(2137);
    }
    int currentLine = 1, i = 0, number;
    int *array = malloc(sizeof(int) * n);
    if(!array) {
        perror("Memory allocation failed!");
        exit(2137);
    }
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

int findLastNode(const char *fileName) {
    const int n4 = numElements(fileName, 4);
    const int n5 = numElements(fileName, 5);
    int *line4 = readLine(fileName, 4, n4);
    int *line5 = readLine(fileName, 5, n5);
    int lastNode;

    for (int i = 0; i < n5; i++) {
        lastNode = line4[line5[i]];
    }

    free(line4);
    free(line5);
    return lastNode;
}

graph createGraph(const char *fileName) {
    graph first = NULL;
    graph temp = NULL;
    graph current = NULL;
    graph prev = NULL;
    const int lastNode = findLastNode(fileName);
    for (int i = 0; i < lastNode; i++) {
        temp = malloc(sizeof(graph));
        if(!temp) {
            perror("CATASTROPHIC FAILURE!!!");
            exit(2137);
        }
        temp->next = NULL;
        temp->numNodes = lastNode;
        if (i == 0) {
            first = temp;
        }
        if (prev == NULL) {
            current = temp;
        }
        else{
            prev->next = temp;
        }
        prev = temp;
    }
    return first;
}

/*Creating a graph*/
graph addEdges(const char *fileName) {
    const int lastNode = findLastNode(fileName);
    const int arraySize = 10; // nwm jaka tu powinna byc wartosc ---
    const int n4 = numElements(fileName, 4);
    const int n5 = numElements(fileName, 5);
    int *line4 = readLine(fileName, 4, n4);
    int *line5 = readLine(fileName, 5, n5);

    graph Graph = createGraph(fileName);

    /*Tu trzeba kontynuwoac */

    free(line4);
    free(line5);
    return Graph;
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
