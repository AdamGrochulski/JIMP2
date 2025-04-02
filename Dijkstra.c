#include "Dijkstra.h"

//Funckja szukająca wartości minimalnej w liście zawierającej wartości ścieżek
int findMinValue(int pathsValues[],bool isNodeCounted[], int nodeCount){
    int min=INT_MAX;
    int wantedIndex;
    for(int i = 0; i<nodeCount; i++){
        if((pathsValues[i] < min) && (isNodeCounted[i]==false)) {
            min = pathsValues[i];
            wantedIndex = i;
        }
    }
    return wantedIndex;
}
//Sprawdzająca, czy przynajmniej został jeden wierzchołek do przeanalizowania
bool evenOneFalse(bool isNodeCounted[], int nodeCount){
    for(int i = 0; i<nodeCount; i++){
        if(isNodeCounted[i]==false)
            return true;
    }
    return false;
}
//Funkcja compare do qsorta
int compare(const void *a, const void *b) {
    const paths *pathA = (const paths *)a;
    const paths *pathB = (const paths *)b;

    if (pathA->value < pathB->value)
        return -1;
    if (pathA->value > pathB->value)
        return 1;
    return 0;
}

//Główna funkcja algorytmu Dijkstry
void algDjikstry(graph Graph, paths *Paths, int startingNode){
    graph takenGraph = Graph;
    int nodeCount = Graph->numNodes;
    //Tablice trzymające czy wierzchołek już został przeanalizowany i wartości ścieżek do startingNode
    bool isNodeCounted[nodeCount];
    int pathsValues[nodeCount];
    //Przypisanie domyślnych wartości do tablic
    for(int i = 0; i < nodeCount; i++){
        pathsValues[i]=INT_MAX; //Zakładamy, że wartość ścieżki do nieprzeanalizowanego wierzchołka jest nieskończenie wielka (w C INT_MAX z biblioteki limits.h)
        isNodeCounted[i]=false; //Status nieprzeanalizowanego wierzchołka = false
    }
    //Przypisanie wartości ścieżki do wierzchołka startowego, jako zero
    pathsValues[startingNode]=0;
    int count = 0;
    //Tutaj analizujemy ścieżki do każdego wierzchołka połączonego z takeNode ( obecnie analizowany wierzchołek)
    while(evenOneFalse(isNodeCounted,nodeCount)==true){
        int takenNodeIndex = findMinValue(pathsValues, isNodeCounted, nodeCount);
        node takenNode = findNode(takenGraph, takenNodeIndex);
        isNodeCounted[takenNodeIndex]=true;
        for(int i = 0; i < takenNode->arraySize; i++){
            int neighbourNodeIndex = takenNode->allEdges[i];
            if(isNodeCounted[neighbourNodeIndex]==false){
                if(pathsValues[neighbourNodeIndex] > pathsValues[takenNodeIndex]){
                    pathsValues[neighbourNodeIndex]=pathsValues[takenNodeIndex]+1;
                }
            }
        }
        count++;
    }
    //Sprawdzanie, czy wierzchołek nie jest przypisany do któregoś z podgrafów, jeśli tak to jego wartość ścieżki zamienia się na zero
    for(int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++){
        node processedNode = findNode(takenGraph, nodeIndex);
        if(processedNode->group != 0){
            pathsValues[nodeIndex]=0;
        }
        Paths[nodeIndex].value=pathsValues[nodeIndex];
        Paths[nodeIndex].nodeID=nodeIndex;
    }
}

void createNodeGroups(graph Graph, int margin, int partition){
    //Odnalezienie liczby wierzchołków i na podstawie tego zamallocowanie pamięci do struktury Paths
    int nodeCount = Graph->numNodes;
    paths *Paths = malloc(nodeCount * sizeof(*Paths));

    //Wylosowanie startowego wierzchołka
    srand(time(NULL));
    int startingNode = rand() % nodeCount;

    // Określenie, jaką wielkość ma pojedyncza grupa (remainder używany dla ostatniej grupy, która ma wielkość: baseGroupSize + remainder)
    int baseGroupSize = nodeCount / partition;
    int remainder = nodeCount % partition;
    
    
    for(int part = 0; part < partition; part++){
        int count = 1;
        int pathIndex = 0;
        //Dla ostatniej grupy zmienienie wielkości grupy
        if(part == partition-1){
            baseGroupSize = baseGroupSize + remainder;
        }
        //Określenie odległości do wierzchołka, z którego zaczynamy tworzenie grupy
        algDjikstry(Graph, Paths, startingNode);
        //Posortowanie rosnąco wartości ścieżek
        qsort(Paths, nodeCount, sizeof(Paths[0]), compare);
        //Dodanie startowego wierzchołka do grupy
        findNode(Graph, startingNode)->group = part + 1;
        while(count<baseGroupSize){
            //Wybieram wierzchołki, które mają wartość różną od zera (już zostały przypisane do innych grup lub wierzchołek startowy)
            if(Paths[pathIndex].value != 0){
                findNode(Graph, Paths[pathIndex].nodeID)->group = part + 1;
                count++;
            }
            pathIndex++;
            //Wybieram następny wierzchołek startowy
            if(part != partition-1){
                startingNode = Paths[pathIndex].nodeID;
            }
        }
    }
    free(Paths);
}