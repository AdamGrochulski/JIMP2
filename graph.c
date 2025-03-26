#include "graph.h"

/*Allocating memory for arrays, assigning nodeID and arraySize*/
node createNode(int arraySize) {
    node newNode = malloc(sizeof(struct node));
    if(!newNode) {
        perror("Memory allocation failed!");
        exit(303);
    }
    newNode->arraySize = arraySize;
    newNode->allEdges  = (arraySize > 0) ? malloc(sizeof(int) * newNode->arraySize) : NULL;
    newNode->divSize = 0;
    newNode->internalEdges = NULL;
    newNode->externalEdges = NULL;
    newNode->difference = 0;
    return newNode;
}

/*Number of elements in a "*.csrrg" file*/
int numElements(const char *fileName, int lineNumber) {
    FILE *file = fopen(fileName, "r");
    if (!file) {
        perror("Cannot open file");
        exit(303);
    }
    int currentLine = 1;
    int count = 0;
    int number;
    while (fscanf(file, "%d", &number) == 1) {
        if (currentLine == lineNumber) {
            count++;
        }
        if (fgetc(file) == '\n')
            currentLine++;
    }
    fclose(file);
    return count;
}

/*function reads the line and returns an array*/
int *readLine(const char *fileName, int lineNumber, int n) {
    FILE *file = fopen(fileName, "r");
    if (!file) {
        perror("Cannot open file");
        exit(303);
    }
    int currentLine = 1, i = 0, number;
    int *array = malloc(sizeof(int) * n);
    if(!array) {
        perror("Memory allocation failed!");
        exit(303);
    }
    while (fscanf(file, "%d", &number) == 1) {
        if (currentLine == lineNumber) {
            array[i++] = number;
            if (i >= n)
                break;
        }
        if (fgetc(file) == '\n')
            currentLine++;
    }
    fclose(file);
    return array;
}

/*Applies only on line 4*/
int findLastNode(const char *fileName) {
    const int n4 = numElements(fileName, 4);
    int *line4 = readLine(fileName, 4, n4);
    int lastNode = 0;

    for (int i = 0; i < n4; i++) {
        if (line4[i] > lastNode) {
            lastNode = line4[i];
        }
    }

    free(line4);
    return lastNode + 1;
}

/*Declaring a graph*/
graph declareGraph(const char *fileName) {
    graph first = NULL;
    graph temp = NULL;
    graph prev = NULL;
    int totalNodes = findLastNode(fileName);
    if (totalNodes <=  0) {
        fprintf(stderr, "Invalid number of nodes: %d\n", totalNodes);
        exit(303);
    }
    for (int i = 0; i < totalNodes; i++) {
        temp = malloc(sizeof(struct graph));
        if(!temp) {
            perror("Memory allocation failed!");
            exit(303);
        }
        temp->next = NULL;
        temp->numNodes = totalNodes;
        if (i == 0) {
            first = temp;
        }
        if (prev != NULL) {
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
}

/*adding all edges to graph and assigning values*/
graph createGraph(const char *fileName) {
    graph Graph = declareGraph(fileName);
    int totalNodes = findLastNode(fileName);
    graph Graph1 = Graph;
    /*Assigning values to the Graph*/
    for (int i = 0; i < totalNodes; i++) {
        Graph1->nodeID = i;
        Graph1->currentNode = createNode(0);
        Graph1 = Graph1->next;
    }

    const int n4 = numElements(fileName, 4);
    const int n5 = numElements(fileName, 5);
    int *line4 = readLine(fileName, 4, n4);
    int *line5 = readLine(fileName, 5, n5);


    for(int i = 0; i < n5; i++) {
        int startIndex = line5[i];
        int endIndex = (i < n5 - 1) ? line5[i+1] : n4;
        if (startIndex >= endIndex) {
            continue;
        }
        int indexDifference = endIndex - startIndex;
        int PK = line4[startIndex];
        graph PKG = findNodeID(Graph, PK);
        if(PKG == NULL) {
            fprintf(stderr, "Cannot find ID: %d\n", PK);
            continue;
        }
        int oldArraySize = PKG->currentNode->arraySize;
        int newArraySize = oldArraySize + indexDifference - 1;
        int *arr = realloc(PKG->currentNode->allEdges, newArraySize * sizeof(int));
        if (arr == NULL) {
            perror("Memory allocation failed!");
            exit(303);
        }
        PKG->currentNode->allEdges = arr;
        for (int j = startIndex + 1; j < endIndex; j++) {
            PKG->currentNode->allEdges[oldArraySize++] = line4[j];
        }
        PKG->currentNode->arraySize = newArraySize;

        for (int k = startIndex + 1; k < endIndex; k++) {
            graph adjacentNode = findNodeID(Graph, line4[k]);
            if(adjacentNode == NULL) {
                fprintf(stderr, "Cannot find ID: %d\n", line4[k]);
                continue;
            }
            int oldAdjacentArraySize = adjacentNode->currentNode->arraySize;
            int newAdjacentArraySize = oldAdjacentArraySize + 1;
            int *arr1 = realloc(adjacentNode->currentNode->allEdges, newAdjacentArraySize * sizeof(int));
            if (arr1 == NULL) {
                perror("Memory allocation failed!");
                exit(303);
            }
            adjacentNode->currentNode->allEdges = arr1;
            adjacentNode->currentNode->allEdges[oldAdjacentArraySize] = PK;
            adjacentNode->currentNode->arraySize = newAdjacentArraySize;
        }
    }
    graph Graph2 = Graph;
    while (Graph2 != NULL) {
        Graph2->numNodes--;
        Graph2 = Graph2->next;
    }
    free(line4);
    free(line5);
    return Graph;
}


void printGraph(graph Graph){
    graph Graph1 = Graph;
    while(Graph1 != NULL) {
        printf("---------------------------\n");
        printf("Node %d: [", Graph1->nodeID);
        for(int i = 0; i < Graph1->currentNode->arraySize; i++) {
            if(i ==  Graph1->currentNode->arraySize - 1) {
                printf("%d] \n", Graph1->currentNode->allEdges[i]);
                printf("Array size: %d\n", Graph1->currentNode->arraySize);
                printf("Num of nodes: %d", Graph1->numNodes);
                printf("\n");
            }else {
                printf("%d,", Graph1->currentNode->allEdges[i]);
            }
        }
        Graph1 = Graph1->next;
    }
    printf("---------------------------\n");
}


void freeGraph(graph Graph) {
    graph current = Graph;
    while (current != NULL) {
        graph temp = current;
        current = current->next;
        free(temp->currentNode);
        free(temp);
    }
}