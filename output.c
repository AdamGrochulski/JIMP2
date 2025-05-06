#include "output.h"

void saveToTxt(graph Graph, const char *inputFile, const char *outputFile, int partition){
    FILE *input = fopen(inputFile, "r");
    if (input==NULL) {
        perror("Cannot open file");
        exit(303);
    }
    FILE *output = fopen(outputFile, "w");
    if (output==NULL) {
        perror("Cannot open file");
        exit(303);
    }

    int matrixSize = 0;
    fscanf(input, "%d", &matrixSize);
    matrixSize--;

    fclose(input);
    
    const int n2 = numElements(inputFile, 2);
    const int n3 = numElements(inputFile, 3);
    int *line2 = readLine(inputFile, 2, n2);
    int *line3 = readLine(inputFile, 3, n3);

    int firstIndex;
    int secondIndex;
    int matrixLine[matrixSize];
    
    for(int index3 = 1; index3 < n3; index3++){
        firstIndex = line3[index3-1];
        secondIndex = line3[index3];
        memset(matrixLine, 0, sizeof matrixLine);
        
        for(int index2 = firstIndex; index2 < secondIndex; index2++){
            matrixLine[line2[index2]] = 1;
        }

        fprintf(output," [");
        for(int matrixRow = 0; matrixRow < matrixSize; matrixRow++){
            fprintf(output,"%d.",matrixLine[matrixRow]);
            if(matrixRow != matrixSize -1){
                fprintf(output," ");
            }
        }
        fprintf(output,"]\n");
        
    }

    int groupCounter = 1;
    int currentNode = 0;
    int connectedNode = 0;
    while(groupCounter <= partition){
        graph takenGraph = Graph; 
        while(takenGraph != NULL) {
            if(groupCounter == takenGraph->currentNode->group){
                currentNode = takenGraph->nodeID;
                for (int i = 0; i < takenGraph->currentNode->internalSize; i++) {
                    connectedNode = takenGraph->currentNode->internalEdges[i];
                    if(connectedNode > currentNode){
                        fprintf(output, "%d - %d\n",currentNode,connectedNode);
                    }
                }
            }
            takenGraph = takenGraph->next;
        }
        if(groupCounter != partition){
            fprintf(output,"\n");
        }
        groupCounter++;
    }

    fclose(output);
}

void saveToBin(graph Graph, const char *inputFile, const char *outputFile){
    
}