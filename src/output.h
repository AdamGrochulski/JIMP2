#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include "graph.h"

void saveToTxt(graph Graph, const char *inputFile, const char *outputFile, int partition);
void saveToBin(graph Graph, const char *inputFile, const char *outputFile, int partition);
int searchMaxColumn(int* line2, int n2);
void writeArrayAsShorts(FILE *file, int *array, int arraySize);
int** findLineSizes(graph Graph, int** lineSizes, int partition);
#endif