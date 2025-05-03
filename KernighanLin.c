#include "KernighanLin.h"
#include "graph.h"
#include "Dijkstra.h"
#include <limits.h>

/*Działanie algorytmu
 * 1. Oblicz D
 * 2. Znaleźć największy zysk dla 2 wierzchołków
 * 3. Mamy wierzchołki A i B
 * 4. Zamieniamy te wierzchołki grupami
 * 5. Zmieniamy dane w wierzchołku A i B InternalSize, InternalEdges, ExternalSize, ExternalEdges
 * 6. Sąsiądy wierzchołków A i B zamieniają wierzchołek A i B w External i Internal Edges, External, Internal Size
 * 7. Ustawiamu locked na 1
 * 8. Działamy dalej, aż rezultaty nie bedą przynosiły zysku
 * 9. Printować wszystkie wierzchołki w odpowienich grupach
 * 10. (?) Tworzymy nowe podgrafy (nowa struktura)(chyba będzie najwygodniej)
 */

/*Szukamy w strukturze Graph wierzchołka o indeksie 'index' */
graph locateNode(graph Graph, int index) {
    graph Graph1 = Graph;
    while(Graph1 != NULL) {
        if(Graph1->nodeID == index) {
            return Graph1;
        }
        Graph1 = Graph1->next;
    }

    fprintf(stderr, ("Cannot find node with such index\n"));
    return NULL;
}


/*Szuka najlepszej grupy dla węzła*/
/*(!) Zastanowic sie czy nad algorytmem szukania wierzchołków dla gorszych grup*/
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


    /**
     *tutaj jeszcze pętla for dla sprawdzenia czy mozna w ogóle
     *rozwazac wierzchołki w tej grupie (czy mają prawidłowe locked)
     * -> Jeśli nie to zwraca gorszą grupę, która może być niezaakceptowana w dalszych
     * krokach algorytmu (zależne od sprawdzenia czy gain dla wybranej pary wynosi wicej niz 0)
     */
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

/*
 * Mamy node A i jej dane
 * Szukamy wierzchołka B, dla którego gain dla A jest największy + locked
 * (!) Zastanowic sie czy nad algorytmem szukania wierzchołków dla gorszych grup
 */

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

    // Mamy ExternalArrayA i ExternalArraySizeA
    int bestGroup = findBestGroup(Graph, graphOrigin);
    int maxDiff = INT_MIN;
    int maxID = -1;
    for(int i = 0; i < Graph->currentNode->externalSize; i++) {
        graph gB = locateNode(graphOrigin, Graph->currentNode->externalEdges[i]);
        if(gB->currentNode->locked == 1) {
            continue;
        }

        int bDiff = countDifference(gB, Graph, graphOrigin);

        if (bDiff>maxDiff && gB->currentNode->group == bestGroup) {
            maxDiff = bDiff;
            maxID = Graph->currentNode->externalEdges[i];
        }
    }

    graph gA = Graph;
    if(maxID == -1) {
        //Nie ma żadnego wierzchołka B do wymiany
        printf("Nie ma żadnego wierzchołka B do wymiany\n");
    }else {
        graph gB = locateNode(graphOrigin, maxID);
        Pair->nodeIndexB = maxID;
        int extSizeB = 0;
        Pair->DifferenceA = countDifference(gA, gB, graphOrigin);
        Pair->DifferenceB = countDifference(gB, gA, graphOrigin);

        /*Pozostało tu przypisac dane wierzchołka B pasujace do wierzchołka A*/
        for(int i = 0; i < gB->currentNode->externalSize; i++) {
            graph tempA = locateNode(graphOrigin, gB->currentNode->externalEdges[i]);
            if(tempA->currentNode->group == gB->currentNode->group) {
                extSizeB++;
            }
        }

        Pair->externalArrayNodeBSize = extSizeB;
        int j = 0;
        Pair->externalArrayNodeB = malloc(sizeof(int) * extSizeB);

        for(int i = 0; i < gB->currentNode->externalSize; i++) {
            graph tempA = locateNode(graphOrigin, gB->currentNode->externalEdges[i]);
            if(tempA->currentNode->group == gB->currentNode->group) {
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






void KernighanLinAlgorithm(graph graphOrigin) {
    int LICZBA = 0;
    while(LICZBA<1){
        graph Graph1 = graphOrigin;
        pair Pair = createPairs(Graph1);

        /*Główna częśc kodu*/
        while(Graph1 != NULL) {
            assignExternalArrayNodeA(Graph1, Pair, graphOrigin);
            adjustNodeB(Graph1, Pair, graphOrigin);
            Graph1 = Graph1->next;
        }

        freePair(Pair);
        LICZBA++;
    }

}
