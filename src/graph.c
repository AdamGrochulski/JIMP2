#include "graph.h"
#include <stdio.h>

/*Allocating memory for arrays, assigning nodeID and arraySize*/
node createNode(int arraySize) {
    node newNode = malloc(sizeof(struct node));
    if(!newNode) {
        perror("Memory allocation failed!");
        exit(303);
    }
    newNode->arraySize = arraySize;
    newNode->allEdges  = (arraySize > 0) ? malloc(sizeof(int) * newNode->arraySize) : NULL;
    newNode->internalSize = 0;
    newNode->externalSize = 0;
    newNode->group = 0;
    newNode->internalEdges = NULL;
    newNode->externalEdges = NULL;
    newNode->difference = 0;
    newNode->locked = 0;
    return newNode;
}

node findNode(graph Graph, int nodeIndex){
    graph takenGraph = Graph;
    int currentIndex = 0;
    while(currentIndex != nodeIndex){
        takenGraph=takenGraph->next;
        currentIndex++;
    }
    return takenGraph->currentNode;
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
    for(int i = 0; i < totalNodes; i++) {
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
    free(line4);
    free(line5);
    return Graph;
}

void printGraph(graph Graph) {
    graph curr = Graph;
    while (curr != NULL) {
        printf("---------------------------\n");
        printf("Node %d: [", curr->nodeID);

        for (int i = 0; i < curr->currentNode->arraySize; i++) {
            if (i == curr->currentNode->arraySize - 1) {
                printf("%d] \n", curr->currentNode->allEdges[i]);
            } else {
                printf("%d, ", curr->currentNode->allEdges[i]);
            }
        }
        
        printf("Array size: %d\n", curr->currentNode->arraySize);
        printf("Group: %d\n", curr->currentNode->group);
        printf("Num of nodes: %d\n", curr->numNodes);

        printf("Internal Size: %d\n", curr->currentNode->internalSize);
        printf("Internal Edges: [");
        for (int i = 0; i < curr->currentNode->internalSize; i++) {
            if (i == curr->currentNode->internalSize - 1) {
                printf("%d", curr->currentNode->internalEdges[i]);
            } else {
                printf("%d, ", curr->currentNode->internalEdges[i]);
            }
        }
        printf("]\n");

        printf("External Size: %d\n", curr->currentNode->externalSize);
        printf("External Edges: [");
        for (int i = 0; i < curr->currentNode->externalSize; i++) {
            if (i == curr->currentNode->externalSize - 1) {
                printf("%d", curr->currentNode->externalEdges[i]);
            } else {
                printf("%d, ", curr->currentNode->externalEdges[i]);
            }
        }
        printf("]\n");
        printf("Difference: %d\n", curr->currentNode->difference);

        curr = curr->next;
    }
    printf("---------------------------\n");
}

void notAlone(graph Graph, int partition){
    graph takenGraph = Graph;
    int takenNodeIndex = 0;
    int aloneNodeIndex = 0;
    int takenNodeGroupIndex = 0;
    int* groupSizes = malloc(sizeof(int) * partition);
    for(int i = 0; i < partition; i++){
        groupSizes[i] = 0;
    }
    graph temp = Graph;
    int groupIndex = 0;
    while(temp != NULL) {
        groupIndex = temp->currentNode->group;
        groupSizes[groupIndex-1] = groupSizes[groupIndex-1] + 1;
        temp = temp->next;
    }

    while(takenGraph != NULL) {
        takenNodeIndex=takenGraph->nodeID;
        takenNodeGroupIndex = takenGraph->currentNode->group - 1;
        if(takenGraph->currentNode->internalSize == 0){
            node tempNode = findNode(Graph,takenGraph->currentNode->allEdges[0]);
            int minGroupSize = groupSizes[tempNode->group - 1];
            int minIndex = takenGraph->currentNode->allEdges[0];
            int minGroupIndex = tempNode->group - 1;
            for(int j = 1; j < takenGraph->currentNode->arraySize; j++){
                int considerIndex = takenGraph->currentNode->allEdges[j];
                node considerNode = findNode(Graph,considerIndex);
                int considerNodeGroup = considerNode->group;
                if(groupSizes[considerNodeGroup - 1] < minGroupSize){
                    minGroupSize = groupSizes[considerNodeGroup - 1];
                    minIndex = considerIndex;
                    minGroupIndex = considerNode->group - 1;
                }
            }
            aloneNodeIndex = minIndex;
            groupSizes[minGroupIndex] = groupSizes[minGroupIndex] - 1;
            groupSizes[takenNodeGroupIndex] = groupSizes[takenNodeGroupIndex] + 1;
            addToInternalEdges(Graph, takenNodeIndex, aloneNodeIndex);
        }
        takenGraph = takenGraph->next;
    }
}

void addToInternalEdges(graph Graph, int takenNodeIndex, int aloneNodeIndex){
    node takenNode = findNode(Graph, takenNodeIndex);
    node aloneNode = findNode(Graph, aloneNodeIndex);

    aloneNode->group = takenNode->group;
    
    int takenCloneSize = takenNode->internalSize;
    int* takenClone = malloc(sizeof(int) * takenCloneSize);
    for (int i = 0; i < takenCloneSize; i++) {
        takenClone[i] = takenNode->internalEdges[i];
    }
    
    int aloneCloneSize = aloneNode->internalSize;
    int* aloneClone = malloc(sizeof(int) * aloneCloneSize);
    for (int i = 0; i < aloneCloneSize; i++) {
        aloneClone[i] = aloneNode->internalEdges[i];
    }

    free(takenNode->internalEdges);
    free(aloneNode->internalEdges);

    takenNode->internalSize = takenNode->internalSize + 1;
    aloneNode->internalSize = aloneNode->internalSize + 1;

    takenNode->internalEdges = malloc(sizeof(int) * takenNode->internalSize);
    for (int i = 0; i < takenNode->internalSize-1; i++) {
        takenNode->internalEdges[i] = takenClone[i];
    }
    takenNode->internalEdges[takenNode->internalSize-1] = aloneNodeIndex;

    aloneNode->internalEdges = malloc(sizeof(int) * aloneNode->internalSize);
    for (int i = 0; i < aloneNode->internalSize-1; i++) {
        aloneNode->internalEdges[i] = aloneClone[i];
    }
    aloneNode->internalEdges[aloneNode->internalSize-1] = takenNodeIndex;

    free(takenClone);
    free(aloneClone);
}

int marginChecker(graph Graph, int partition, int margin){
    graph takenGraph = Graph;
    int* groupSizes = malloc(sizeof(int) * partition);
    for(int i = 0; i < partition; i++){
        groupSizes[i] = 0;
    }
    int groupIndex = 0;
    while(takenGraph != NULL) {
        groupIndex = takenGraph->currentNode->group;
        groupSizes[groupIndex-1] = groupSizes[groupIndex-1] + 1;
        takenGraph = takenGraph->next;
    }

    double max = groupSizes[0];
    double min = groupSizes[0];

    for(int i = 0; i < partition; i++){
        if(groupSizes[i]>max){
            max = groupSizes[i];
        }
        else if(groupSizes[i]<min){
            min = groupSizes[i];
        }
    }

    double currentMargain = (( max / min ) - 1)*100;
    double ceilMargain = ceil(currentMargain);

    free(groupSizes);
    return (int)ceilMargain;
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

