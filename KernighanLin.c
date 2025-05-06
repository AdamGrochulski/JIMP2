#include "KernighanLin.h"
#include "graph.h"
#include "Dijkstra.h"
#include <limits.h>

static graph *searchNodeTable = NULL;
static int searchNodeTableSize = 0;

/*Inicjalizacja wskaźnika searchNodeTable na tablicę wskaźników do wierzchołków grafu*/
void initializeNodeLookupTable(graph graphOrigin) {
    graph temp = graphOrigin;
    searchNodeTableSize = graphOrigin->numNodes;
    searchNodeTable = malloc((searchNodeTableSize + 1) * sizeof(graph));
    for (int i = 0; i <= searchNodeTableSize; i++)
        searchNodeTable[i] = NULL;
    temp = graphOrigin;
    while (temp != NULL) {
        searchNodeTable[temp->nodeID] = temp;
        temp = temp->next;
    }
}


/*Zwracamy wskaźnik o danym indeksie*/
graph locateNode(graph Graph, int index) {
    if (index < 0 || index > searchNodeTableSize || searchNodeTable[index] == NULL) {
        fprintf(stderr, "Cannot find node with such index\n");
        return NULL;
    }
    return searchNodeTable[index];
}


/*Zwolnienie pamięci*/
void freeNodeLookupTable() {
    free(searchNodeTable);
    searchNodeTable = NULL;
    searchNodeTableSize = 0;
}


/*Szuka najlepszej grupy dla węzła*/
int findBestGroup(graph Graph, graph graphOrigin) {
    /*Tabela licznosci grup połączonych z węzłem
     * 1 wiersz - indeks grupy
     * 2 wiersz - ile wierzchołków ma dany indeks grupy
     */
    int groupCardianality[Graph->currentNode->externalSize][2];
    for (int i = 0; i < Graph->currentNode->externalSize; i++) {
        groupCardianality[i][0] = -1;
        groupCardianality[i][1] = 0;
    }

    for(int i = 0; i < Graph->currentNode->externalSize; i++) {
        graph g = locateNode(graphOrigin, Graph->currentNode->externalEdges[i]);
        if(groupCardianality[i][0] == -1) {
            int isPossible = 1;

            for (int j = 0; j < i; j++) {
                /*Jeśli już jest grupa o danym indeksie*/
                if (groupCardianality[j][0] == g->currentNode->group) {
                    isPossible = 0;
                    break;
                }
            }
            /*Przypisanie grupy jeśli jej nie ma w tablicy*/
            if (isPossible == 1) {
                groupCardianality[i][0] = g->currentNode->group;
            }
        }
    }

    /*Dodajemy liczności grup*/
    for(int i = 0; i < Graph->currentNode->externalSize; i++) {
        graph g = locateNode(graphOrigin, Graph->currentNode->externalEdges[i]);
        for (int j = 0; j < Graph->currentNode->externalSize && groupCardianality[j][0] != -1; j++) {
            if (g->currentNode->group == groupCardianality[j][0]) {
                groupCardianality[j][1]++;
            }
        }
    }

    int max = 0;
    int maxIDGroup = -1;

    /*Szukamy maksyamlnej licznosci grupy*/
    for (int i = 0; i < Graph->currentNode->externalSize; i++) {
        if(groupCardianality[i][1] > max) {
            max = groupCardianality[i][1];
            maxIDGroup = groupCardianality[i][0];
        }
    }

    // if(maxIDGroup == -1) {
    //     fprintf(stderr, "Błąd w szukaniu najlepszej grupy dla węzła\n");
    //     exit(1);
    // }

    int potentialCandidates[Graph->currentNode->externalSize];
    for (int i = 0; i < Graph->currentNode->externalSize; i++) {
        potentialCandidates[i] = -1;
    }

    int Integer = 0;
    for (int i = 0; i < Graph->currentNode->externalSize; i++) {
        graph g = locateNode(graphOrigin, Graph->currentNode->externalEdges[i]);
        if (g->currentNode->group == maxIDGroup && g->currentNode->locked == 0) {
            return maxIDGroup;
        } else if (g->currentNode->group != maxIDGroup && g->currentNode->locked == 0) {
            potentialCandidates[Integer++] = Graph->currentNode->externalEdges[i];
        }
    }

    //Jeśli wszystkie wierzchołki są zablokowane, to zwracamy najgorszą grupę;
    if(Integer == 0) {
        int minGroupID = maxIDGroup;
        int minGroupCard = INT_MAX;
        for(int i = 0; i < Graph->currentNode->externalSize; i++) {
            if(groupCardianality[i][1] < minGroupCard) {
                minGroupID = groupCardianality[i][0];
            }
        }
        return minGroupID;
    }else {
        //Zwracamy najlepszą grupę z możliwych kandydatów do zamiany
        int card = 0;
        for(int i = 0; i < Integer; i++) {
            graph g = locateNode(graphOrigin, potentialCandidates[i]);
            if (groupCardianality[i][0] == g->currentNode->group) {
                if (card > groupCardianality[i][1]) {
                    card = groupCardianality[i][1];
                    maxIDGroup = groupCardianality[i][0];
                }
            }
        }
        return maxIDGroup;
    }
}


/* Szuka ExternalArraySize dla najlepszej grupy z wierzchołka*/
int getExternalArraySize(graph Graph, graph graphOrigin) {
    int bestGroup = findBestGroup(Graph, graphOrigin);
    int extSize = 0;

    for(int i = 0; i < Graph->currentNode->externalSize; i++) {
        graph g = locateNode(graphOrigin, Graph->currentNode->externalEdges[i]);
        if(g->currentNode->group == bestGroup) {
            extSize++;
        }
    }

    return extSize;
}


/*Wpisujemy do Pair->externalArray wierzchołki z najlepszej grupy + jej rozmiar*/
void assignExternalArrayNodeA(graph Graph, pair Pair, graph graphOrigin) {
    int extSize = getExternalArraySize(Graph, graphOrigin);
    int bestGroup = findBestGroup(Graph, graphOrigin);
    Pair->nodeIndexA = Graph->nodeID;
    Pair->externalArrayNodeA = malloc(sizeof(int) * extSize);
    Pair->externalArrayNodeASize = extSize;
    int j = 0;

    for(int i = 0; i < Graph->currentNode->externalSize && j < extSize; i++) {
        graph g = locateNode(graphOrigin, Graph->currentNode->externalEdges[i]);
        if (g->currentNode->group == bestGroup) {
            Pair->externalArrayNodeA[j++] = Graph->currentNode->externalEdges[i];
        }
    }
}


/*Liczmy difference dla danej pary*/
int countDifference(graph gB, graph gA, graph graphOrigin) {
    int externalSize = 0;
    for (int i = 0; i < gB->currentNode->externalSize; i++) {
        graph neighbor = locateNode(graphOrigin, gB->currentNode->externalEdges[i]);
        if (neighbor && neighbor->currentNode->group == gA->currentNode->group) {
            externalSize++;
        }
    }
    return externalSize - gB->currentNode->internalSize;
}


/*Obliczamy gain dla pary*/
int countGain(pair Pair) {
    return Pair->DifferenceB + Pair->DifferenceA - 2;
}


/*Dopasowujemy Wierzchołek B tak, aby difference dla pary był jak największy*/
void adjustNodeB(graph Graph, pair Pair, graph graphOrigin) {
    int bestGroup = findBestGroup(Graph, graphOrigin);
    int maxDiff = INT_MIN;
    int maxID = -1;

    // Wybieramy kandydata jako wierzchołek B
    for (int i = 0; i < Graph->currentNode->externalSize; i++) {
        graph candidate = locateNode(graphOrigin, Graph->currentNode->externalEdges[i]);
        if (candidate->currentNode->locked == 1) {
            continue;
        }
        int bDiff = countDifference(candidate, Graph, graphOrigin);
        if (bDiff > maxDiff && candidate->currentNode->group == bestGroup) {
            maxDiff = bDiff;
            maxID = Graph->currentNode->externalEdges[i];
        }
    }

    graph gA = Graph;
    if(maxID == -1) {
        // Brak kandydata w danej grupie
        //fprintf(stderr, "Brak dostępnego kandydata B do zamiany\n");
        return;
    } else {
        graph gB = locateNode(graphOrigin, maxID);
        Pair->nodeIndexB = maxID;
        Pair->DifferenceA = countDifference(gA, gB, graphOrigin);
        Pair->DifferenceB = countDifference(gB, gA, graphOrigin);

        int extSizeB = 0;
        for (int i = 0; i < gB->currentNode->externalSize; i++) {
            graph neighbor = locateNode(graphOrigin, gB->currentNode->externalEdges[i]);
            if (neighbor->currentNode->group == gA->currentNode->group) {
                extSizeB++;
            }
        }

        Pair->externalArrayNodeBSize = extSizeB;
        int j = 0;
        Pair->externalArrayNodeB = malloc(sizeof(int) * extSizeB);
        for (int i = 0; i < gB->currentNode->externalSize; i++) {
            graph neighbor = locateNode(graphOrigin, gB->currentNode->externalEdges[i]);
            if (neighbor->currentNode->group == gA->currentNode->group) {
                Pair->externalArrayNodeB[j++] = gB->currentNode->externalEdges[i];
            }
        }

        Pair->gain = countGain(Pair);
    }
}



/*Inicjalizacja pojedynczej pary*/
pair initializePair() {
    pair Pair = malloc(sizeof(struct pair));
    Pair->nodeIndexA = 0;
    Pair->nodeIndexB = 0;
    Pair->gain = 0;
    Pair->externalArrayNodeA = NULL;
    Pair->externalArrayNodeASize = 0;
    Pair->externalArrayNodeB = NULL;
    Pair->externalArrayNodeBSize = 0;
    Pair->DifferenceA = 0;
    Pair->DifferenceB = 0;
    Pair->id = 0;
    return Pair;
}


/*Inicjalizacja listy liniowej par*/
pair createPairs(graph graphOriginal) {
    graph tempGraph = graphOriginal;
    pair head = NULL;
    pair tail = NULL;

    while (tempGraph != NULL) {
        pair newPair = initializePair();
        newPair->id = tempGraph->nodeID;
        newPair->next = NULL;

        if (head == NULL) {
            head = newPair;
            tail = newPair;
        } else {
            tail->next = newPair;
            tail = newPair;
        }

        tempGraph = tempGraph->next;
    }
    return head;
}


/*Zwalnianie pamięci dla struktury Pair */
void freePair(pair Pair) {
    pair current = Pair;
    while (current != NULL) {
        pair temp = current;
        current = current->next;
        free(temp->externalArrayNodeA);
        free(temp->externalArrayNodeB);
        free(temp);
    }
}


/*Znajdowanie najlepszej pary do zamiany*/
pair findBestPair(pair Pair) {
    if (Pair == NULL) {
        fprintf(stderr, "Lista par jest niezainicjalizowana\n");
        exit(1);
    }
    pair bestPair = Pair;
    pair current = Pair;
    int maxGain = current->gain;

    while (current != NULL) {
        if (current->gain > maxGain) {
            maxGain = current->gain;
            bestPair = current;
        }
        current = current->next;
    }

    return bestPair;
}


/*Printowanie najlepszej pary*/
void printSwap(pair Pair, graph graphOrigin) {
    if (Pair == NULL) {
        fprintf(stderr, "Błąd: Pointer do Pair jest NULL.\n");
        return;
    }
    if (graphOrigin == NULL) {
        fprintf(stderr, "Błąd: Pointer do graphOrigin jest NULL.\n");
        return;
    }

    graph gA = locateNode(graphOrigin, Pair->nodeIndexA);
    graph gB = locateNode(graphOrigin, Pair->nodeIndexB);

    printf("----------------------------------\n");
    printf("Informacje o Pair:\n");
    printf("ID: %d\n", Pair->id);
    printf("Node A Index: %d\n", Pair->nodeIndexA);
    printf("Node B Index: %d\n", Pair->nodeIndexB);
    printf("Gain: %d\n", Pair->gain);
    printf("DifferenceA: %d\n", Pair->DifferenceA);
    printf("DifferenceB: %d\n", Pair->DifferenceB);

    printf("External Array dla Node A (rozmiar: %d): ", Pair->externalArrayNodeASize);
    if (Pair->externalArrayNodeA != NULL && Pair->externalArrayNodeASize > 0) {
        printf("[");
        for (int i = 0; i < Pair->externalArrayNodeASize; i++) {
            if (i == Pair->externalArrayNodeASize - 1) {
                printf("%d", Pair->externalArrayNodeA[i]);
            } else {
                printf("%d, ", Pair->externalArrayNodeA[i]);
            }
        }
        printf("]\n");
    } else {
        printf("Brak danych\n");
    }

    // Drukowanie External Array dla Node B
    printf("External Array dla Node B (rozmiar: %d): ", Pair->externalArrayNodeBSize);
    if (Pair->externalArrayNodeB != NULL && Pair->externalArrayNodeBSize > 0) {
        printf("[");
        for (int i = 0; i < Pair->externalArrayNodeBSize; i++) {
            if (i == Pair->externalArrayNodeBSize - 1) {
                printf("%d", Pair->externalArrayNodeB[i]);
            } else {
                printf("%d, ", Pair->externalArrayNodeB[i]);
            }
        }
        printf("]\n");
    } else {
        printf("Brak danych\n");
    }

    puts("-----");

    // Drukowanie informacji dla Node A
    if (gA != NULL) {
        printf("Node A (ID: %d):\n", gA->nodeID);
        printf("Grupa: %d\n", gA->currentNode->group);
        printf("InternalSize: %d\n", gA->currentNode->internalSize);
        printf("InternalEdges: [");
        if (gA->currentNode->internalSize > 0) {
            for (int i = 0; i < gA->currentNode->internalSize; i++) {
                if (i == gA->currentNode->internalSize - 1) {
                    printf("%d", gA->currentNode->internalEdges[i]);
                } else {
                    printf("%d, ", gA->currentNode->internalEdges[i]);
                }
            }
            printf("]\n");
        } else {
            printf("]\n");
        }
        printf("ExternalSize: %d\n", gA->currentNode->externalSize);
        printf("ExternalEdges: [");
        // Używamy prawidłowego warunku dla externalEdges w Node A
        if (gA->currentNode->externalSize > 0) {
            for (int i = 0; i < gA->currentNode->externalSize; i++) {
                if (i == gA->currentNode->externalSize - 1) {
                    printf("%d", gA->currentNode->externalEdges[i]);
                } else {
                    printf("%d, ", gA->currentNode->externalEdges[i]);
                }
            }
            printf("]\n");
        } else {
            printf("]\n");
        }
        printf("Difference: %d\n", gA->currentNode->difference);
    } else {
        printf("Node A nie został znaleziony w grafie.\n");
    }

    puts("-----");

    if (gB != NULL) {
        printf("Node B (ID: %d):\n", gB->nodeID);
        printf("Grupa: %d\n", gB->currentNode->group);
        printf("InternalSize: %d\n", gB->currentNode->internalSize);
        printf("InternalEdges: [");
        if (gB->currentNode->internalSize > 0) {
            for (int i = 0; i < gB->currentNode->internalSize; i++) {
                if (i == gB->currentNode->internalSize - 1) {
                    printf("%d", gB->currentNode->internalEdges[i]);
                } else {
                    printf("%d, ", gB->currentNode->internalEdges[i]);
                }
            }
            printf("]\n");
        } else {
            printf("]\n");
        }
        printf("ExternalSize: %d\n", gB->currentNode->externalSize);
        printf("ExternalEdges: [");
        if (gB->currentNode->externalSize > 0) {
            for (int i = 0; i < gB->currentNode->externalSize; i++) {
                if (i == gB->currentNode->externalSize - 1) {
                    printf("%d", gB->currentNode->externalEdges[i]);
                } else {
                    printf("%d, ", gB->currentNode->externalEdges[i]);
                }
            }
            printf("]\n");
        } else {
            printf("]\n");
        }
        printf("Difference: %d\n", gB->currentNode->difference);
    } else {
        printf("Node B nie został znaleziony w grafie.\n");
    }

    printf("----------------------------------\n");
}


/*Zamiana wierzchołków A i B między grupami*/
void swapAB(pair Pair, graph graphOrigin) {
    if (Pair->nodeIndexA == Pair->nodeIndexB) {
        return;
    }
    graph gA = locateNode(graphOrigin, Pair->nodeIndexA);
    graph gB = locateNode(graphOrigin, Pair->nodeIndexB);

    // A
    int* builderInternalA;
    int* builderExternalA;

    int intA = Pair->externalArrayNodeASize;
    builderInternalA = malloc(sizeof(int) * intA);
    for (int i = 0; i < intA; i++) {
        builderInternalA[i] = Pair->externalArrayNodeA[i];
    }

    int newSizeA = (gA->currentNode->externalSize - intA) + gA->currentNode->internalSize;
    builderExternalA = malloc(sizeof(int) * newSizeA);

    int countA = 0;
    for (int j = 0; j < gA->currentNode->externalSize; j++) {
        int skip = 0;
        for (int i = 0; i < intA; i++) {
            if (gA->currentNode->externalEdges[j] == Pair->externalArrayNodeA[i]) {
                skip = 1;
                break;
            }
        }
        if (!skip) {
            builderExternalA[countA++] = gA->currentNode->externalEdges[j];
        }
    }

    for (int k = 0; k < gA->currentNode->internalSize; k++) {
        builderExternalA[countA++] = gA->currentNode->internalEdges[k];
    }
    int extA = countA;

    int* builderInternalB;
    int* builderExternalB;

    // B:
    int intB = Pair->externalArrayNodeBSize;
    builderInternalB = malloc(sizeof(int) * intB);
    for (int i = 0; i < intB; i++) {
        builderInternalB[i] = Pair->externalArrayNodeB[i];
    }

    int newSizeB = (gB->currentNode->externalSize - intB) + gB->currentNode->internalSize;
    builderExternalB = malloc(sizeof(int) * newSizeB);

    int countB = 0;
    for (int j = 0; j < gB->currentNode->externalSize; j++) {
        int skip = 0;
        for (int i = 0; i < intB; i++) {
            if (gB->currentNode->externalEdges[j] == Pair->externalArrayNodeB[i]) {
                skip = 1;
                break;
            }
        }
        if (!skip) {
            builderExternalB[countB++] = gB->currentNode->externalEdges[j];
        }
    }
    for (int k = 0; k < gB->currentNode->internalSize; k++) {
        builderExternalB[countB++] = gB->currentNode->internalEdges[k];
    }
    int extB = countB;

    free(gA->currentNode->internalEdges);
    gA->currentNode->internalEdges = malloc(sizeof(int) * intA);
    for (int i = 0; i < intA; i++) {
        gA->currentNode->internalEdges[i] = builderInternalA[i];
    }
    gA->currentNode->internalSize = intA;

    free(gA->currentNode->externalEdges);
    gA->currentNode->externalEdges = malloc(sizeof(int) * extA);
    for (int i = 0; i < extA; i++) {
        gA->currentNode->externalEdges[i] = builderExternalA[i];
    }
    gA->currentNode->externalSize = extA;

    // Zamiana grup między węzłami i ustawienie flagi (blokady) dla węzła A
    int tempGrA = gA->currentNode->group;
    gA->currentNode->group = gB->currentNode->group;
    gA->currentNode->locked = 1;
    gB->currentNode->group = tempGrA;

    // Aktualizacja węzła B:
    free(gB->currentNode->internalEdges);
    gB->currentNode->internalEdges = malloc(sizeof(int) * intB);
    for (int i = 0; i < intB; i++) {
        gB->currentNode->internalEdges[i] = builderInternalB[i];
    }
    gB->currentNode->internalSize = intB;

    free(gB->currentNode->externalEdges);
    gB->currentNode->externalEdges = malloc(sizeof(int) * extB);
    for (int i = 0; i < extB; i++) {
        gB->currentNode->externalEdges[i] = builderExternalB[i];
    }
    gB->currentNode->externalSize = extB;

    free(builderInternalA);
    free(builderExternalA);
    free(builderInternalB);
    free(builderExternalB);
}


/*Warunek sprawdzający, czy dalej można liczyć*/
void updateGainStatus(pair Pair, int *Condition, graph graphOrigin) {
    int allLocked = 1;
    for (graph temp = graphOrigin; temp != NULL; temp = temp->next) {
        if (temp->currentNode->locked == 0) {
            allLocked = 0;
            break;
        }
    }

    pair BestPair = findBestPair(Pair);
    // Jeśli najlepsza para ma gain <= 0, lub wszystkie węzły są locked, kończymy iteracje.
    if (allLocked || (BestPair != NULL && BestPair->gain <= 0)) {
        *Condition = 0;
    }
}


/*Printowanie określonego wierzchołka*/
void printNode(graph curr) {
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
    printf("locked: %d\n", curr->currentNode->locked);
}


/* Algorytm Kernighana-Lina*/
void KernighanLinAlgorithm(graph graphOrigin) {
    int iterations = 0;
    int Condition = 1;

    while (iterations < graphOrigin->numNodes && Condition) {
        graph Graph1 = graphOrigin;
        pair Pair = createPairs(Graph1);

        /*przetwarzamy każdą parę */
        pair currentPair = Pair;
        while (Graph1 != NULL && currentPair != NULL) {
            assignExternalArrayNodeA(Graph1, currentPair, graphOrigin);
            adjustNodeB(Graph1, currentPair, graphOrigin);

            Graph1 = Graph1->next;
            currentPair = currentPair->next;
        }

        updateGainStatus(Pair, &Condition, graphOrigin);
        // if (Condition == 0) {
        //     freePair(Pair);
        //     break;
        // }

        pair BestPair = findBestPair(Pair);

        printSwap(BestPair, graphOrigin);
        printNode(locateNode(graphOrigin, BestPair->nodeIndexA));
        printNode(locateNode(graphOrigin, BestPair->nodeIndexB));

        swapAB(BestPair, graphOrigin);

        printf("<========ZAMIANA=========>\n");
        printNode(locateNode(graphOrigin, BestPair->nodeIndexA));
        printNode(locateNode(graphOrigin, BestPair->nodeIndexB));

        freePair(Pair);

        //printf("Iteration: %d\n", iterations);
        iterations++;
    }
    calcDiff(graphOrigin);
}


/*Obliczanie difference dla wierzchołka*/
void calcDiff(graph graphOrigin) {
    graph temp = graphOrigin;
    while(temp != NULL) {
        temp->currentNode->difference = temp->currentNode->externalSize - temp->currentNode->internalSize;
        temp = temp->next;
    }
}


/*wyświetlanie wyników*/
void displayResults(graph graphOrigin) {
    graph temp = graphOrigin;
    int positive = 0;
    int negative = 0;

    while(temp != NULL) {
        if (temp->currentNode->internalSize >= temp->currentNode->externalSize - 1) {
            positive++;
        }else {
            negative++;
        }
        temp = temp->next;
    }

    printf("Pozytywny rezultat: %d \n", positive);
    printf("Negatywny rezultat: %d \n", negative);
}


/*Wyświetlanie węzłów, które są zablokowane*/
void printLocked(graph graphOrigin) {
    graph temp = graphOrigin;
    while(temp != NULL) {
        if(temp->currentNode->locked == 1) {
            printNode(temp);
        }
        temp = temp->next;
    }
}