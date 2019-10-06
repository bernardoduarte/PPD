#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "Matrix.h"

int main(int argc, char **argv)
{
    int numtasks, rank, tag = 1;
    char *filenameA, *filenameB, *filenameC;
    unsigned int i, j;
    unsigned int rowsA, columnsA;
    unsigned int rowsB, columnsB;
    unsigned int rowsC, columnsC;
    unsigned int resultsSize;
    double *matrixA, *matrixB, *matrixC;
    double *results;
    double buffer;
    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {

        if (argc < 4) {
            printf("Error missing command line argument.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        filenameA = argv[1];
        filenameB = argv[2];
        filenameC = argv[3];

        if (Matrix_from_file(&matrixA, &rowsA, &columnsA, filenameA) != 0) {
            printf("Error reading matrix A.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        if (Matrix_from_file(&matrixB, &rowsB, &columnsB, filenameB) != 0) {
            printf("Error reading matrix B.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        if (columnsA != rowsB) {
            printf("Error invalid matrix multiplication (%u,%u)x(%u,%u).\n", rowsA, columnsA, rowsB, columnsB);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (i = 1; i < numtasks; ++i) {
            printf("Sending to %d\n", i);
            MPI_Send(&rowsA, 1, MPI_UNSIGNED, i, tag, MPI_COMM_WORLD);
            printf("Sent A rows\n");
            MPI_Send(&columnsA, 1, MPI_UNSIGNED, i, tag, MPI_COMM_WORLD);
            printf("Sent A columns\n");
            MPI_Send(matrixA, rowsA * columnsA, MPI_DOUBLE, i, tag, MPI_COMM_WORLD);
            printf("Sent matrix A\n");
            MPI_Send(&rowsB, 1, MPI_UNSIGNED, i, tag, MPI_COMM_WORLD);
            printf("Sent B rows\n");
            MPI_Send(&columnsB, 1, MPI_UNSIGNED, i, tag, MPI_COMM_WORLD);
            printf("Sent B columns\n");
            MPI_Send(matrixB, rowsB * columnsB, MPI_DOUBLE, i, tag, MPI_COMM_WORLD);
            printf("Sent matrix B\n");
        }
    }
    else {
        MPI_Recv(&rowsA, 1, MPI_UNSIGNED, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("%d Received A rows\n", rank);
        MPI_Recv(&columnsA, 1, MPI_UNSIGNED, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("%d Received A columns\n", rank);
        if (Matrix_initialize(&matrixA, rowsA, columnsA) != 0) {
            printf("Error initializing matrix A.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        MPI_Recv(matrixA, rowsA * columnsA, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("%d Received matrix A\n", rank);

        MPI_Recv(&rowsB, 1, MPI_UNSIGNED, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("%d Received B rows\n", rank);
        MPI_Recv(&columnsB, 1, MPI_UNSIGNED, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("%d Received B columns\n", rank);
        if (Matrix_initialize(&matrixB, rowsB, columnsB) != 0) {
            printf("Error initializing matrix B.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        MPI_Recv(matrixB, rowsB * columnsB, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("%d Received matrix B\n", rank);
    }

    rowsC = rowsA;
    columnsC = columnsB;
    resultsSize = (rowsC * columnsC) / numtasks;
    if ((rowsC * columnsC % numtasks) > rank) {
        resultsSize++;
    }
    results = calloc(resultsSize, sizeof(*results));
    if (results == NULL) {
        printf("Error allocating results.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    for (i = rank; i < rowsC * columnsC; i += numtasks) {
        unsigned int row, column;
        row = i / columnsC;
        column = i % columnsC;
        for (j = 0; j < columnsA; ++j) {
            double valueA, valueB;
            Matrix_get(matrixA, rowsA, columnsA, row, j, &valueA);
            Matrix_get(matrixB, rowsB, columnsB, j, column, &valueB);
            *(results + (i / numtasks)) += valueA * valueB;
        }
    }

    if (rank == 0) {
        Matrix_initialize(&matrixC, rowsC, columnsC);

        for(i = 0; i < (rowsC * columnsC - resultsSize); ++i) {
            unsigned int row, column;
            int source;
            MPI_Recv(&row, 1, MPI_UNSIGNED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            source = status.MPI_SOURCE;
            MPI_Recv(&column, 1, MPI_UNSIGNED, source, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&buffer, 1, MPI_DOUBLE, source, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            Matrix_set(&matrixC, rowsC, columnsC, row, column, buffer);
        }
        Matrix_print(matrixC, rowsC, columnsC);

        for (i = rank; i < rowsC * columnsC; i += numtasks) {
            unsigned int row, column;
            row = i / columnsC;
            column = i % columnsC;
            Matrix_set(&matrixC, rowsC, columnsC, row, column, *(results + (i / numtasks)));
        }

        Matrix_to_file(matrixC, rowsC, columnsC, filenameC);
        Matrix_print(matrixC, rowsC, columnsC);

        Matrix_finalize(&matrixC);
    }
    else {
        for (i = rank; i < rowsC * columnsC; i += numtasks) {
            unsigned int row, column;
            row = i / columnsC;
            column = i % columnsC;
            MPI_Send(&row, 1, MPI_UNSIGNED, 0, tag, MPI_COMM_WORLD);
            MPI_Send(&column, 1, MPI_UNSIGNED, 0, tag, MPI_COMM_WORLD);
            MPI_Send(results + (i / numtasks), 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
        }
    }

    Matrix_finalize(&matrixA);
    Matrix_finalize(&matrixB);
    free(results);

    MPI_Finalize();

    return 0;
}