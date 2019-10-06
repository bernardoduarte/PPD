#ifndef _H_MATRIX_
#define _H_MATRIX_

#include <stdio.h>
#include <stdlib.h>

double rand_double(double min, double max)
{
    return (max - min) * ((double)rand() / (double)RAND_MAX) + min;
}

unsigned int Matrix_initialize(double **matrix, unsigned int rows, unsigned int columns)
{
    (*matrix) = calloc(rows * columns, sizeof(**matrix));
    if ((*matrix) == NULL) {
        return 1;
    }
    return 0;
}

unsigned int Matrix_finalize(double **matrix)
{
    free(*matrix);
    return 0;
}


unsigned int Matrix_set(double **matrix, unsigned int rows, unsigned int columns, unsigned int row, unsigned int column, double value)
{
    *((*matrix) + columns * row + column) = value;
    return 0;
}

unsigned int Matrix_get(double *matrix, unsigned int rows, unsigned int columns, unsigned int row, unsigned int column, double *value)
{
    *value = *(matrix + columns * row + column);
    return 0;
}

unsigned int Matrix_print(double *matrix, unsigned int rows, unsigned int columns)
{
    unsigned int i, j;
    double value;
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < columns; ++j) {
            Matrix_get(matrix, rows, columns, i, j, &value);
            printf("%lf ", value);
        }
        printf("\n");
    }
    return 0;
}

unsigned int Matrix_from_file(double **matrix, unsigned int *rows, unsigned int *columns, char *filename)
{
    unsigned int i, j;
    double value;
    FILE* fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        return 1;
    }
    fscanf(fp, "%u %u\n", rows, columns);
    if (Matrix_initialize(matrix, *rows, *columns) != 0) {
        return 1;
    }
    for (i = 0; i < *rows; ++i) {
        for (j = 0; j < *columns; ++j) {
            fscanf(fp, "%lf ", &value);
            Matrix_set(matrix, *rows, *columns, i, j, value);
        }
        fscanf(fp, "\n");
    }
    fclose(fp);
    return 0;
}

unsigned int Matrix_to_file(double *matrix, unsigned int rows, unsigned int columns, char *filename)
{
    unsigned int i, j;
    double value;
    FILE *fp;
    fp = fopen(filename, "w+");
    if (fp == NULL) {
        return 1;
    }
    fprintf(fp, "%u %u\n", rows, columns);
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < columns; ++j) {
            Matrix_get(matrix, rows, columns, i, j, &value);
            fprintf(fp, "%lf ", value);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    return 0;
}

unsigned int Matrix_randomize(double **matrix, unsigned int rows, unsigned int columns, double min, double max)
{
    unsigned int i, j;
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < columns; ++j) {
            Matrix_set(matrix, rows, columns, i, j, rand_double(min, max));
        }
    }
    return 0;
}

#endif
