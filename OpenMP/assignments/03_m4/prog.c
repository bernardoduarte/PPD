#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "timer.h"

int randInt(int min, int max)
{
        return (rand() % (max - min + 1)) + min;
}

int * randIntArray(unsigned int length, int min, int max)
{
        int *array;
        unsigned int i;

        array = malloc(sizeof(*array) * length);
        for (i = 0; i < length; ++i) {
                array[i] = randInt(min, max);
        }

        return array;
}

void printIntArray(int *array, unsigned int length, FILE* stream)
{
        unsigned int i;
        for (i = 0; i < length; ++i) {
                fprintf(stream, "%02d ", array[i]);
        }
}

void printlnIntArray(int *array, unsigned int length, FILE* stream)
{
        printIntArray(array, length, stream);
        fprintf(stream, "\n");
}

int main(int argc, char const *argv[])
{
        int numThreads;
        unsigned int length;
        int minRand;
        int maxRand;
        double mean;
        int mode;
        int minimumValue;
        int maximumValue;
        int *values;
        double inicio, fim, tempo;
        unsigned int i;

        if (argc < 3) {
                printf("Error missing command line argument.\n");
                return 1;
        }

        length = atoi(argv[1]);
        minRand = atoi(argv[2]);
        maxRand = atoi(argv[3]);
        numThreads = atoi(argv[4]);

        values = randIntArray(length, minRand, maxRand);

        GET_TIME(inicio);

        #pragma omp parallel sections num_threads(numThreads) private(i) shared(values)
        {
                #pragma omp section
                {
                        printf("Thread num: %d\n", omp_get_thread_num());
                        mean = 0;
                        for (i = 0; i < length; ++i) {
                                mean += values[i];
                        }
                        mean /= (double)length;
                }

                // #pragma omp section
                // {
                //         printf("Thread num: %d\n", omp_get_thread_num());
                //         unsigned int *counts;
                //         counts = malloc(sizeof(*counts) * (maxRand - minRand + 1));
                //         memset(counts, 0, sizeof(*counts) * (maxRand - minRand + 1));
                //         mode = values[0];
                //         counts[values[0] - minRand]++;
                //         for (i = 1; i < length; ++i) {
                //                 counts[values[i] - minRand]++;
                //                 if (counts[values[i] - minRand] > counts[mode - minRand]) {
                //                         mode = values[i];
                //                 }
                //         }
                //         free(counts);
                // }

                #pragma omp section
                {
                        printf("Thread num: %d\n", omp_get_thread_num());
                        minimumValue = maxRand;
                        for (i = 0; i < length; ++i) {
                                if (values[i] < minimumValue) {
                                        minimumValue = values[i];
                                }
                        }
                }

                #pragma omp section
                {
                        printf("Thread num: %d\n", omp_get_thread_num());
                        maximumValue = minRand;
                        for (i = 0; i < length; ++i) {
                                if (values[i] > maximumValue) {
                                        maximumValue = values[i];
                                }
                        }
                }
        }

        GET_TIME(fim);
        tempo = fim - inicio;
        printf("Tempo: %.8lf\n", tempo);

        printf("Mean: %02.2lf\n", mean);
        printf("Mode: %d\n", mode);
        printf("Min: %d\n", minimumValue);
        printf("Max: %d\n", maximumValue);

        free(values);

        return 0;
}