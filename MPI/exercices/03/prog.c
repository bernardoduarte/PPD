#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int numtasks, rank, tag = 1, size, tests;
    char *filename;
    double *buffer, time;
    MPI_Status status;
    FILE *fp;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (numtasks <= 1) {
        printf("Error number of tasks should be greater than 1.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (argc < 4) {
        printf("Error missing command line argument.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    tests = atoi(argv[1]);
    size = atoi(argv[2]);

    buffer = malloc(size * sizeof(*buffer));
    for (int i = 0; i < size; ++i) {
        buffer[i] = -1.0;
    }

    if (rank == 0) {
        filename = argv[3];

        fp = fopen(filename, "w+");

        if (fp == NULL) {
            printf("Error opening file.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        time = MPI_Wtime();

        for (int i = 0; i < tests; ++i) {
            for (int j = 0; j < size; ++j) {
                buffer[j] = 2.0;
            }
            MPI_Send(buffer, size, MPI_DOUBLE, 1, tag, MPI_COMM_WORLD);
            MPI_Recv(buffer, size, MPI_DOUBLE, 1, tag, MPI_COMM_WORLD, &status);
        }

        fprintf(fp, "%lf\n", MPI_Wtime() - time);

        fclose(fp);
    }
    else {
        for (int i = 0; i < tests; ++i) {
            MPI_Recv(buffer, size, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);
            for (int j = 0; j < size; ++j) {
                buffer[j] = 4.0;
            }
            MPI_Send(buffer, size, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
        }
    }

    free(buffer);

    MPI_Finalize();
    return 0;
}