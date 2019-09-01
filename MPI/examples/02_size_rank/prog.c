#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
        int numtasks, rank, rc;

        MPI_Init(&argc, &argv);

        MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        printf("Number of tasks = %d. My rank = %d.\n", numtasks, rank);

        MPI_Finalize();

        return 0;
}
