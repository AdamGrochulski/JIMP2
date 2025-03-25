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
graph declareGraph(const char *fileName) {
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

graph searchNodeID(graph Graph, int nodeID) {
    graph Graph1 = Graph;

    while(Graph1->nodeID != nodeID) {
        if(Graph1->nodeID == nodeID) {
            return Graph1;
        }
        Graph1 = Graph1->next;
    }

    return NULL;
}

/*adding all edges to graph and assigning values*/
graph createGraph(const char *fileName) {
    graph Graph = declareGraph(fileName);
    const int lastNode = findLastNode(fileName);
    const int arraySize = 10; // nwm jaka tu powinna byc wartosc ---
    const int n4 = numElements(fileName, 4);
    const int n5 = numElements(fileName, 5);
    int *line4 = readLine(fileName, 4, n4);
    int *line5 = readLine(fileName, 5, n5);
    int allEdgesArraySize;
    graph Graph1 = Graph;

    /*Assigning nodeID to the Graph*/
    for (int i = 0; i < lastNode; i++) {
        Graph1->nodeID = i;
        Graph1->currentNode->arraySize = 0;
        Graph1 = Graph1->next;
    }
    Graph1 = Graph;

    node newNode = NULL;
    int n = 0;
    graph Graph2 = NULL;
    int PK;
    int *arr;
    int *res;

    for(int i = 0; i < n4; i++) {
        //Najpierw stworzmy graph gdzie bedzie nameID dla wszystkich wierzcholków (easy) ok
        for(int j = 0; j < n5; j++) {
            if (i == line4[line5[j]]) { // jesli indeks i oraz indeks z lini 4 (PK) pasuje
                //jesli jakis wezel ma juz jakies dane!
                PK = i;
                if (Graph1->currentNode->arraySize == 0) {
                    Graph1->nodeID = line4[line5[j]];
                    n = line5[j+1] - line5[j] - 1;
                    newNode = createNode(n);
                    /*Wpisanie 72;39;91;4;54 do allEdegs */
                    newNode->allEdges = malloc(sizeof(int) * n);
                    for (int k = 0; k < n; k++) {
                        newNode->allEdges[k] = line4[1 + line5[j] + k]; //chyba ok  trzeba jeszcze sprawdzic
                    }
                    Graph1->currentNode = newNode;
                }else {
                    arr = Graph1->currentNode->allEdges;
                    n = line5[j+1] - line5[j] - 1;
                    Graph1->currentNode->arraySize+n;
                    res = realloc(arr, sizeof(int) * Graph1->currentNode->arraySize);
                    if (res == NULL) {
                        free(arr);
                        exit(2137);
                    }
                    arr = res;
                    for (int k = Graph1->currentNode->arraySize - n, l = 0 ; l < n; k++, l++) {
                        arr[k] = line4[1 + line5[j] + l];
                    }
                    arr = Graph1->currentNode->allEdges;
                    // reallocowac pamiec
                    // musimy przejsc do konca tablicy
                    // dodac te wartosci
                }
                Graph1 = Graph1->next;
            }else {
                //Musimy wpisac 72;39;91;4;54 do allEdegs ok
                //Stworzyc / sprawdzic czy istenija  72;39;91;4;54 - 2 przypadki
                // nie stowrzyc tylko przejsc do 72 i dopisac tam 0 (bo node istnieje);
                //trzeba jeszcze przejsc na koniec tablicy
                if (Graph1->currentNode->arraySize == 0) {
                    Graph2 = searchNodeID(Graph1, i);
                    Graph2->currentNode->allEdges = malloc(sizeof(int));
                    if(!Graph2->currentNode->allEdges) {
                        perror("Memory allocation failed!");
                        exit(2137);
                    }
                    arr = Graph2->currentNode->allEdges;

                    arr[0] = PK;
                    Graph2->currentNode->allEdges = arr;
                }else {
                    Graph2 = searchNodeID(Graph1, i);
                    arr = Graph2->currentNode->allEdges;
                    Graph2->currentNode->arraySize++;

                    res = realloc(Graph2->currentNode->allEdges, Graph2->currentNode->arraySize * sizeof(int));
                    if (res == NULL) {
                        free(arr);
                        exit(2137);
                    }
                    arr[Graph2->currentNode->arraySize-1] = PK;
                    Graph2->currentNode->allEdges = arr;
                }
                // ----- Do  72;39;91;4;54 wpisac 0 (PK)  -----
                // Szukaj(currentNode: 72) --> idziemy do node --> Realloc o jeden element  ---> dodajemy 0
                // Szukaj(currentNode: 39) --> idziemy do node --> Realloc o jeden element  ---> dodajemy 0
                // Szukaj(currentNode: 91) --> idziemy do node --> Realloc o jeden element  ---> dodajemy 0
                // Szukaj(currentNode: 4) --> idziemy do node --> Realloc o jeden element  ---> dodajemy 0
                // Szukaj(currentNode: 54) --> idziemy do node --> Realloc o jeden element  ---> dodajemy 0
                //                            (2-3) Graph1->currentNode->allEdges (realloc)
            }
        }
    }

    arr = NULL;
    res = NULL;
    free(line4);
    free(line5);
    return Graph;
}

void printGraph(graph Graph) {
    graph Graph1 = Graph;
    while(Graph1 != NULL) {
        printf("%d: [", Graph1->nodeID);
        for(int i = 0; i < Graph1->currentNode->arraySize; i++) {
            if(i ==  Graph1->currentNode->arraySize - 1) {
                printf("%d ]\n", Graph1->currentNode->allEdges[i]);
            }else {
                printf("%d, ", Graph1->currentNode->allEdges[i]);
            }
        }
        Graph1 = Graph1->next;
    }
}


int main(int argc, char *argv[]){
    const char *fileName = "graf.csrrg";
    graph Graph = createGraph(fileName);

    printGraph(Graph);

    return 0;
}
