#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

/*FILE graf.csrrg
 * Linijki 1-3 potrzebne do macierzy
 * Linijki 2-5 potrzebne do grafu
 */

//calloc node->arraysize 0
//realloc 1

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


/*Allocating memory for arrays, assigning nodeID and arraySize*/
node createNode(int arraySize) {
    node newNode = malloc(sizeof(node));
    if(!newNode) {
        perror("Memory allocation failed!");
        exit(2137);
    }
    newNode->arraySize = arraySize;
    /*newNode->divSize = arraySize/2;  divSize trzeba będzie zmodyfikowac*/
    newNode->allEdges = malloc(sizeof(int) * newNode->arraySize);
    //newNode->internalEdges = malloc(sizeof(int) * newNode->divSize);
    //newNode->externalEdges = malloc(sizeof(int) * newNode->divSize);
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

/*Creating a graph*/
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
        temp->currentNode->arraySize = 0; /* setting all arraySize to 0 */
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


/*Finding specific from linkedList node based on nodeID*/
graph findNodeID(graph Miautoni, int nodeID) {
    graph Graph = Miautoni;
    while(Graph != NULL) {
        if(Graph->nodeID == nodeID) {
            return Graph;
        }
        Graph = Graph->next;
    }

    return NULL;
};


/*adding all edges to graph and assigning values*/
graph addEdges(const char *fileName) {
    graph Graph = createGraph(fileName);
    const int lastNode = findLastNode(fileName);
    const int arraySize = 10; // nwm jaka tu powinna byc wartosc ---
    const int n4 = numElements(fileName, 4);
    const int n5 = numElements(fileName, 5);
    int *line4 = readLine(fileName, 4, n4);
    int *line5 = readLine(fileName, 5, n5);
    int allEdgesArraySize;
    graph Graph1 = Graph;


    for(int i = 0; i < n5; i++) {
        if(n5 != i++) {
            Graph1->nodeID = line4[line5[i]];
            allEdgesArraySize = line5[i++] - line5[i] - 1;
            for(int j = 0; j < allEdgesArraySize; j++) {
                Graph1->nodeID = line4[line5[i]];
                if(Graph1->currentNode->arraySize == 0 && j == 0) /*PRIMARY NODE NIE BYLO WCZESNIEJ*/{
                    //Stworzyc wezel
                    //Umiescicic go w strcu graph
                    node newNode = createNode(allEdgesArraySize);
                    Graph1->currentNode = newNode;
                    Graph1->currentNode->allEdges[] = line4[line5[i]+1+j]; //allEdges[ ]
                }else if (Graph1->currentNode->arraySize == 0 && j != 0) /*FOREIGN NODE NIE BYLO WCZESNIEJ*/{
                    node newNode = createNode(1);
                    Graph1->currentNode = newNode;
                    Graph1->currentNode->allEdges[j] = line4[line5[i]+1+j]; // issue - allEdges[j]
                    /*to be continued*/
                }else if (Graph1->currentNode->arraySize != 0 && j == 0)/*PRIMARY NODE BYLO WCZESNIEJ*/{
                    /*to be continued*/
                }else if (Graph1->currentNode->arraySize != 0 && j != 0)/*FOREIGN NODE BYLO WCZESNIEJ*/ {
                    /*to be continued*/
                }
            Graph1 = Graph1->next;
        }
    }

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
