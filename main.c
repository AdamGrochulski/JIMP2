#include "graph.h"

int main(int argc, char *argv[]){
    const char *fileName = "graf.csrrg";
    graph Graph = createGraph(fileName);
    printGraph(Graph);
    freeGraph(Graph);

    return 0;
}
