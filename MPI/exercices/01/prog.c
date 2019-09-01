#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int numtasks, rank, input, output, source, dest, tag=1;
    char *filename;
    MPI_Status status;
    FILE *fp;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        if (argc < 3) {
            printf("Error missing command line argument.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        input = atoi(argv[1]);
        filename = argv[2];

        fp = fopen(filename, "w+");
        if (fp == NULL) {
            printf("Error opening file.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fprintf(fp, "identificacao = %d valor %d\n", rank, input);
        output = input + rank;
        dest = (rank + 1) % numtasks;
        if (numtasks > 1) {
            MPI_Send(&input, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
            for (int i = 1; i < numtasks; ++i) {
                source = i % numtasks;
                MPI_Recv(&input, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
                fprintf(fp, "Identificacao = %d valor %d\n", source, input);
            }
        }

        fclose(fp);
    }
    else {
        source = rank - 1;
        MPI_Recv(&input, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        output = input + rank;
        dest = (rank + 1) % numtasks;
        if (rank != numtasks - 1) {
            MPI_Send(&output, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
        }
        MPI_Send(&output, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}
