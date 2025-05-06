//Biblioteki standardowe
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>

//Biblioteki programu sliceIt
#include "graph.h"
#include "Dijkstra.h"
#include "KernighanLin.h"
#include "output.h"

//Funkcja do zmienia koloru tekstu (zrobiona w celach estetycznych)
void Colors(int type) {
    if (type == 0) //Zmiana koloru na domyślny
        printf("\033[0m");
    if (type == 1) //Zmiana koloru na niebieski
        printf("\033[1;34m");
    if (type == 2) //Zmiana koloru na zielony
        printf("\033[1;32m");
    if (type == 3) //Zmiana koloru na czerwony
        printf("\033[1;31m");
    if (type >= 4) //Zmiana koloru na fioletowy
        printf("\033[1;35m");
    if (type == 5) //Zmiana koloru na żółty
        printf("\033[1;33m");
}


bool allVariablesChecker(char *inputFile, char *outputFile, int margin, int partition){
    int errorCounter = 0;
    if(inputFile==NULL){
        printf("Brak pliku wejściowego!\n");
        errorCounter++;
    }

    if(outputFile==NULL){
        printf("Brak pliku wyjściowego!\n");
        errorCounter++;
    }
    if(margin==-1){
        printf("Brak podanego marginesu!\n");
        errorCounter++;
    }
    if(partition==-1){
        printf("Brak podanej ilości oczekiwanych podgrafów!\n");
        errorCounter++;
    }
    if(errorCounter!=0){
        return false;
    }
    return true;
}   

int main(int argc, char *argv[]) {
    Colors(0); //Ustawienie koloru tekstu na domyślny

    //Zmienne używane przez getopt
    int opt;

    //Zmienne związane z plikiem wyjściowym i wejściowym
    char *inputFile = NULL;
    char *outputFile = NULL;

    //Zmienne służące do dzielenia grafu
    int margin = -1; // Liczba -1 to stan nieprzetworzenia zmiennej
    int partition = -1;

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"output", required_argument, 0, 'o'},
        {"input", required_argument, 0, 'i'},
        {"margin", required_argument, 0, 'm'},
        {"partition", required_argument, 0, 'p'},
        {"force", no_argument, 0, 'f'},
        {"mforce", no_argument, 0, 'k'},
        {"pforce", no_argument, 0, 'l'},
        {"verbose", no_argument, 0, 'v'},
        {"txt", no_argument, 0, 't'},
        {"bin", no_argument, 0, 'b'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "hvo:i:m:p:fklvtb", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                printf("\n");
                printf("Instrukcja obsługi użytkownika "); Colors(5); printf("sliceIt"); Colors(0); printf(" :\n");
                Colors(2); printf("-h --help"); Colors(0); printf(" : wyświetla dostępne argumenty wywołania "); Colors(5); printf("sliceIt\n"); Colors(0);
                Colors(2); printf("-i --input"); Colors(0); printf(" : określa, w którym pliku są dane wejściowe\n");
                Colors(2); printf("-o --output"); Colors(0); printf(" : określa do jakiego pliku zostają zapisane podgrafy\n");
                Colors(2); printf("-m --margin"); Colors(0); printf(" : określa margines procentowy\n");
                Colors(2); printf("-p --partition"); Colors(0); printf(" : określa na ile części ma zostać podzielony graf\n");
                Colors(2); printf("-f --force"); Colors(0); printf(" : pozwala podzielić graf z zaniedbaniem marginesu procentowego oraz podzielić na dowolną ilość części\n");
                Colors(2); printf("--mforce"); Colors(0); printf(" : pozwala podzielić graf z zaniedbaniem marginesu procentowego\n");
                Colors(2); printf("--pforce"); Colors(0); printf(" : pozwala podzielić graf z zaniedbaniem konkretnej ilości podgrafów na które ma być podzielony graf\n");
                Colors(2); printf("-v --verbose"); Colors(0); printf(" : wyświetla dodatkowe szczegóły podczas pracy programu\n");
                Colors(2); printf("-t --txt"); Colors(0); printf(" : zapisuje wyjście programu do pliku tekstowego\n");
                Colors(2); printf("-b --bin"); Colors(0); printf(" : zapisuje wyjście programu do pliku binarnego\n");
                printf("\n");
                return EXIT_SUCCESS;
            
            case 'i':
                inputFile = optarg;
                FILE *input = fopen(inputFile, "r");
                if(input==NULL){
                    printf("Błąd wczytywania pliku wejściowego!\n");
                    return EXIT_FAILURE;
                }
                break;
            
            case 'o':
                outputFile = optarg;
                break;

                break;

            case 'm':
                margin = strtol(optarg, NULL, 10);
                if(margin < 0 || margin > 100){
                    printf("Margines procentowy poza zakresem [0,100]\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'p':
                partition = atoi(optarg);
                if(partition <= 0){
                    printf("Nie można podzielić grafu na niedodatnią liczbę!\n");
                    return EXIT_FAILURE;
                }
                break;

            case 'f':
                margin = 0;
                partition = 2;
                break;
            case 'k':
                margin = 0;
                break;
            case 'l':
                partition = 2;
                break;

            case 'v':
                //Tutaj będzie tryb deweloperski ( będzie wyświetlał przebieg działania programu )
                break;
            case 't':
                //Tutaj będzie tryb zapisywania pliku w formacie tekstowym ( domyślnie to by był format csrrg )
                break;
            case 'b':
                //Tutaj będzie tryb zapisywania pliku w binarnym ( domyślnie to by był format csrrg )
                break;

            case '?':

                return EXIT_FAILURE;
        }
    }


    //Sprawdzamy wartości potrzebne do poprawnego działania programu
    if(allVariablesChecker(inputFile,outputFile,margin,partition)==false){
        return EXIT_FAILURE;
    }

    //Przebieg działania programu sliceIt
    graph Graph = createGraph(inputFile);
    createNodeGroups(Graph,margin,partition);
    subarray(Graph);
    KernighanLinAlgorithm(Graph);
    printGraph(Graph);
    saveToTxt(Graph,inputFile,outputFile,partition);
    freeGraph(Graph);
    return EXIT_SUCCESS;
}
