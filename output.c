#include "output.h"

void saveToTxt(graph Graph, const char *inputFile, const char *outputFile, int partition){
    FILE *output = fopen(outputFile, "w");
    if (output==NULL) {
        perror("Cannot open file");
        exit(303);
    }
    
    const int n2 = numElements(inputFile, 2);
    const int n3 = numElements(inputFile, 3);
    int *line2 = readLine(inputFile, 2, n2);
    int *line3 = readLine(inputFile, 3, n3);

    int firstIndex;
    int secondIndex;
    int matrixSize = searchMaxColumn(line2,n2) + 1;

    //Tu zooptymalizować
    for(int index3 = 1; index3 < n3; index3++){
        firstIndex = line3[index3-1];
        secondIndex = line3[index3];
        int* matrixLine = malloc(sizeof(int) * (matrixSize));
        for(int j = 0; j < matrixSize; j++){
            matrixLine[j] = 0;
        }
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
        free(matrixLine);
        
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

void saveToBin(graph Graph, const char *inputFile, const char *outputFile, int partition){
    FILE *output = fopen(outputFile, "wb");
    if (output==NULL) {
        perror("Cannot open file");
        exit(303);
    }

    const int n2 = numElements(inputFile, 2);
    const int n3 = numElements(inputFile, 3);
    int *line1 = readLine(inputFile, 1, 1);
    int *line2 = readLine(inputFile, 2, n2);
    int *line3 = readLine(inputFile, 3, n3);

    writeArrayAsShorts(output, line1, 1);
    writeArrayAsShorts(output, line2, n2);
    writeArrayAsShorts(output, line3, n3);

    free(line1);
    free(line2);
    free(line3);

    graph takenGraph = Graph;
    int** lineSizes = malloc( partition * sizeof(int *));
    lineSizes = findLineSizes(Graph, lineSizes, partition);
    //Array zawierający arraye wszystkich grup, w których są zawarte linie do zapisania!
    int*** lines = malloc( partition * sizeof(int **));

    //Pomocnicze listy zawierające liczniki
    int* firstLineCounter = malloc(partition * sizeof(int));
    int* secondLineCounter = malloc(partition * sizeof(int));

    for(int i = 0; i < partition; i++){
        lines[i]=malloc(2 * sizeof(int **));
        lines[i][0]=malloc(lineSizes[i][0] * sizeof(int *));
        lines[i][1]=malloc(lineSizes[i][1] * sizeof(int *));
        firstLineCounter[i] = 0;
        secondLineCounter[i] = 0;
    }

    while(takenGraph != NULL) {
        int currentNodeIndex = takenGraph->nodeID;
        int currentGroup = takenGraph->currentNode->group;
        lines[currentGroup-1][0][firstLineCounter[currentGroup-1]] = currentNodeIndex;
        lines[currentGroup-1][1][secondLineCounter[currentGroup-1]] = takenGraph->currentNode->internalSize + 1;
        firstLineCounter[currentGroup-1] = firstLineCounter[currentGroup-1] + 1;
        secondLineCounter[currentGroup-1] = secondLineCounter[currentGroup-1] + 1;
        for(int internalIndex = 0; internalIndex < takenGraph->currentNode->internalSize; internalIndex++){
            if(takenGraph->currentNode->internalEdges[internalIndex] > currentNodeIndex){
                lines[currentGroup-1][0][firstLineCounter[currentGroup-1]] = takenGraph->currentNode->internalEdges[internalIndex];
                firstLineCounter[currentGroup-1] = firstLineCounter[currentGroup-1] + 1;
            }
        }
        takenGraph = takenGraph->next;
    }

    free(firstLineCounter);
    free(secondLineCounter);

    for(int groupIndex = 0; groupIndex < partition; groupIndex++){
        writeArrayAsShorts(output, lines[groupIndex][0], lineSizes[groupIndex][0]);
        writeArrayAsShorts(output, lines[groupIndex][1], lineSizes[groupIndex][1]);
        free(lines[groupIndex][0]);
        free(lines[groupIndex][1]);
        free(lines[groupIndex]);
        free(lineSizes[groupIndex]);
    }
    free(lineSizes);
    free(lines);

    fclose(output);
}

int searchMaxColumn(int* line2, int n2){
    int max = 0;
    for(int i = 0; i < n2; i++){
        if(line2[i] > max){
            max = line2[i];
        }
    }
    return max;
}

void writeArrayAsShorts(FILE *file, int *array, int arraySize){
    // unsigned short seperator = -1;
    fwrite(&arraySize, sizeof(unsigned short), 1, file);
    for(int i = 0; i < arraySize; i++){
        unsigned short temp = (short)array[i];
        fwrite(&temp, sizeof(unsigned short), 1, file);
    }
    
}

int** findLineSizes(graph Graph, int** lineSizes, int partition){
    graph takenGraph = Graph;
    for(int i = 0; i < partition; i++){
        lineSizes[i] = malloc(2 * sizeof(int));
        lineSizes[i][0] = 0;
        lineSizes[i][1] = 0;
    }
    while(takenGraph != NULL) {
        int currentNodeIndex = takenGraph->nodeID;
        int currentGroup = takenGraph->currentNode->group;
        lineSizes[currentGroup-1][0] = lineSizes[currentGroup-1][0] + 1;
        for(int internalIndex = 0; internalIndex < takenGraph->currentNode->internalSize; internalIndex++){
            if(takenGraph->currentNode->internalEdges[internalIndex] > currentNodeIndex){
                lineSizes[currentGroup-1][0] = lineSizes[currentGroup-1][0] + 1;
            }
        }
        lineSizes[currentGroup-1][1] = lineSizes[currentGroup-1][1] + 1;
        takenGraph = takenGraph->next;
    }
    return lineSizes;
}