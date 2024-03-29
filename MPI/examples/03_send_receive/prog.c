#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
    {
    int numtasks, rank, dest, source, rc, count, tag=1;
    char inmsg, outmsg;
    MPI_Status Stat;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        dest = 1;
        source = 1;
        outmsg = 'x';
        MPI_Send(&outmsg, 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
        MPI_Recv(&inmsg, 1, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);
    }
    else if (rank == 1) {
        dest = 0;
        source = 0;
        outmsg = 'y';
        MPI_Recv(&inmsg, 1, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);
        MPI_Send(&outmsg, 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    }

    MPI_Get_count(&Stat, MPI_CHAR, &count);
    printf("Task %d: Received %d char(s) from task %d with tag %d inmsg=%c.\n", rank, count, Stat.MPI_SOURCE, Stat.MPI_TAG, inmsg);

    MPI_Finalize();
    return 0;
}
