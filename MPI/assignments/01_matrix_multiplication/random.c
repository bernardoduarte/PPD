#include <stdlib.h>
#include <time.h>
#include "Matrix.h"

int main(int argc, char **argv)
{
    char *filename;
    unsigned int rows, columns;
    double min, max;
    double *matrix;

    srand(time(NULL));

    rows = atoi(argv[1]);
    columns = atoi(argv[2]);
    min = (double)atoi(argv[3]);
    max = (double)atoi(argv[4]);
    filename = argv[5];

    Matrix_initialize(&matrix, rows, columns);

    Matrix_randomize(&matrix, rows, columns, min, max);

    Matrix_to_file(matrix, rows, columns, filename);

    Matrix_finalize(&matrix);

    return 0;
}