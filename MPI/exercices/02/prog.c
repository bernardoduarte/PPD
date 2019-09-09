#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int numtasks, rank, input, output, tag=1;
    char *filename;
    MPI_Status status;
    FILE *fp;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (numtasks <= 1) {
        printf("Error number of tasks should be greater than 1.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0) {
        if (argc < 2) {
            printf("Error missing command line argument.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        filename = argv[1];

        fp = fopen(filename, "w+");
        if (fp == NULL) {
            printf("Error opening file.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (int i = 0; i < 100 * (numtasks - 1); ++i) {
            MPI_Recv(&input, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            fprintf(fp, "%d\n", status.MPI_SOURCE);
        }

        fclose(fp);
    }
    else {
        for (int i = 0; i < 100; ++i) {
            MPI_Send(&output, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
