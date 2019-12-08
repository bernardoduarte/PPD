#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

unsigned int boardInit(unsigned char ***board, unsigned int rows, unsigned int columns)
{
    unsigned int i;
    (*board) = malloc(sizeof(**board) * rows);
    if ((*board) == NULL) {
        return 1;
    }
    for (i = 0; i < rows; ++i) {
        (*board)[i] = malloc(sizeof(***board) * columns);
        if ((*board)[i] == NULL) {
            return 1;
        }
        memset((*board)[i], 0, columns);
    }
    return 0;
}

void boardDestroy(unsigned char ** board, unsigned int rows, unsigned int columns)
{
    unsigned int i;
    for (i = 0; i < rows; ++i) {
        free(board[i]);
    }
    free(board);
}

unsigned int boardFromFile(unsigned char ***board, unsigned int *rows, unsigned int *columns, char *filename)
{
    unsigned int i, j;
    char value;
    FILE* fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        return 1;
    }
    fscanf(fp, "%u %u\n", rows, columns);
    if (boardInit(board, *rows, *columns) != 0) {
        return 1;
    }
    for (i = 0; i < *rows; ++i) {
        for (j = 0; j < *columns; ++j) {
            fscanf(fp, "%c", &value);
            (*board)[i][j] = (value == 'x') ? 1 : 0;
        }
        fscanf(fp, "\n");
    }
    fclose(fp);
    return 0;
}

unsigned int boardToFile(unsigned char **board, unsigned int rows, unsigned int columns, char *filename)
{
    unsigned int i, j;
    char value;
    FILE* fp;
    fp = fopen(filename, "w+");
    if (fp == NULL) {
        return 1;
    }
    fprintf(fp, "%u %u\n", rows, columns);
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < columns; ++j) {
            value = board[i][j] ? 'x' : '.';
            fprintf(fp, "%c", value);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    return 0;
}

unsigned int boardPrint(unsigned char **board, unsigned int rows, unsigned int columns)
{
    unsigned int i, j;
    char value;
    printf("%u %u\n", rows, columns);
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < columns; ++j) {
            value = board[i][j] ? 'x' : '.';
            printf("%c", value);
        }
        printf("\n");
    }
    return 0;
}

unsigned int taskGetNumRows(unsigned int taskNum, unsigned int numtasks, unsigned int rows)
{
    unsigned int numRows = rows / numtasks;
    if (taskNum < rows % numtasks) {
        numRows++;
    }
    return numRows;
}

unsigned int taskGetInitialRow(unsigned int taskNum, unsigned int numtasks, unsigned int rows)
{
    unsigned int initialRow = taskNum * (rows / numtasks);
    if (rows % numtasks) {
        initialRow += (rows % numtasks > taskNum) ? taskNum : rows % numtasks;
    }
    return initialRow;
}

unsigned int countNeighbors(unsigned char **board, unsigned int rows, unsigned int columns, unsigned int row, unsigned int column)
{
    unsigned int count = 0;

    if (row > 0 && column > 0 && board[row - 1][column - 1]) count++;

    if (row > 0 && board[row - 1][column]) count++;

    if (row > 0 && column < (columns - 1) && board[row - 1][column + 1]) count++;

    if (column > 0 && board[row][column - 1]) count++;

    if (column < (columns - 1) && board[row][column + 1]) count++;

    if (row < (rows - 1) && column > 0 && board[row + 1][column - 1]) count++;

    if (row < (rows - 1) && board[row + 1][column]) count++;

    if (row < (rows - 1) && column < (columns - 1) && board[row + 1][column + 1]) count++;

    return count;
}

int main(int argc, char **argv)
{
    unsigned int numtasks, rank, tag = 1;
    char *inputFile, *outputFile;
    unsigned int iterations;
    unsigned char **board;
    unsigned char **auxBoard;
    unsigned int rows, columns;
    unsigned int h, i, j;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Initialize
    if (rank == 0) {

        if (argc < 4) {
            printf("Error missing command line argument.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        inputFile = argv[1];
        outputFile = argv[2];
        iterations = atoi(argv[3]);
        printf("Input: %s\nOutput: %s\nIterations: %d\n", inputFile, outputFile, iterations);

        if (boardFromFile(&board, &rows, &columns, inputFile) != 0) {
            printf("Error reading input file: %s\n", inputFile);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        if (boardInit(&auxBoard, rows, columns) != 0) {
            printf("Error initializing task %u auxiliary board\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        
        for (i = 1; i < numtasks; ++i) {
            MPI_Send(&rows, 1, MPI_UNSIGNED, i, tag, MPI_COMM_WORLD);
            MPI_Send(&columns, 1, MPI_UNSIGNED, i, tag, MPI_COMM_WORLD);
            MPI_Send(&iterations, 1, MPI_UNSIGNED, i, tag, MPI_COMM_WORLD);
        }
        unsigned int numRows = taskGetNumRows(rank, numtasks, rows);
        printf("[%d]: NumRows: %d, Padding: %d\n", rank, numRows, 1);

        for (i = 1; i < numtasks; ++i) {
            unsigned int initialRow = taskGetInitialRow(i, numtasks, rows);
            unsigned int numRows = taskGetNumRows(i, numtasks, rows);
            printf("[%d]: Transfering %d work rows starting at %d row with paddings to %d\n", rank, numRows, initialRow, i);

            for (j = 0; j < numRows; ++j) {
                MPI_Send(board[initialRow + j], columns, MPI_UNSIGNED_CHAR, i, tag, MPI_COMM_WORLD);
            }
            // Upper padding
            MPI_Send(board[initialRow - 1], columns, MPI_UNSIGNED_CHAR, i, tag, MPI_COMM_WORLD);
            // Lower padding
            if (i < numtasks - 1) {
                MPI_Send(board[initialRow + numRows], columns, MPI_UNSIGNED_CHAR, i, tag, MPI_COMM_WORLD);
            }
        }
    }
    else {
        MPI_Recv(&rows, 1, MPI_UNSIGNED, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&columns, 1, MPI_UNSIGNED, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&iterations, 1, MPI_UNSIGNED, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // unsigned int initialRow = taskGetInitialRow(rank, numtasks, rows);
        unsigned int numRows = taskGetNumRows(rank, numtasks, rows);
        unsigned int padding = (rank < numtasks - 1) ? 2 : 1;
        printf("[%d]: NumRows: %d, Padding: %d\n", rank, numRows, padding);

        if (boardInit(&board, numRows + padding, columns) != 0) {
            printf("Error initializing task %u board\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        if (boardInit(&auxBoard, numRows + padding, columns) != 0) {
            printf("Error initializing task %u auxiliary board\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (i = 1; i < numRows + 1; ++i) {
            MPI_Recv(board[i], columns, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        // Upper padding
        MPI_Recv(board[0], columns, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // Lower padding
        if (rank < numtasks - 1) {
            MPI_Recv(board[numRows + 1], columns, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    // Iterate
    if (rank == 0) {
        unsigned int numRows = taskGetNumRows(rank, numtasks, rows);
        unsigned int padding = (numtasks > 1) ? 1 : 0;
        // printf("[%d]: NumRows: %d, Padding: %d\n", rank, numRows, padding);
        for (h = 0; h < iterations; ++h) {
            for (i = 0; i < numRows; ++i) {
                for (j = 0; j < columns; ++j) {
                    unsigned int neighbors = countNeighbors(board, numRows + padding, columns, i, j);
                    if (board[i][j] == 0) {
                        if (neighbors == 3) {
                            auxBoard[i][j] = 1;
                        }
                        else {
                            auxBoard[i][j] = 0;
                        }
                    }
                    else {
                        if (neighbors < 2 || neighbors > 3) {
                            auxBoard[i][j] = 0;
                        }
                        else {
                            auxBoard[i][j] = 1;
                        }
                    }
                }
            }

            if (numtasks > 1) {
                MPI_Send(auxBoard[numRows - 1], columns, MPI_UNSIGNED_CHAR, rank + 1, tag, MPI_COMM_WORLD);
                MPI_Recv(auxBoard[numRows], columns, MPI_UNSIGNED_CHAR, rank + 1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            for (i = 0; i < numRows + padding; ++i) {
                for (j = 0; j < columns; ++j) {
                    board[i][j] = auxBoard[i][j];
                }
            }
        }
    }
    else{
        unsigned int numRows = taskGetNumRows(rank, numtasks, rows);
        unsigned int padding = (rank < numtasks - 1) ? 2 : 1;
        for (h = 0; h < iterations; ++h) {
            for (i = 1; i < numRows + padding; ++i) {
                for (j = 0; j < columns; ++j) {
                    unsigned int neighbors = countNeighbors(board, numRows + padding, columns, i, j);
                    if (board[i][j] == 0) {
                        if (neighbors == 3) {
                            auxBoard[i][j] = 1;
                        }
                        else {
                            auxBoard[i][j] = 0;
                        }
                    }
                    else {
                        if (neighbors < 2 || neighbors > 3) {
                            auxBoard[i][j] = 0;
                        }
                        else {
                            auxBoard[i][j] = 1;
                        }
                    }
                }
            }

            unsigned int initialRow = taskGetInitialRow(rank, numtasks, rows);
            // printf("[%d]: Receiving row %d from task %d\n", rank, initialRow - 1, rank - 1);
            MPI_Recv(auxBoard[0], columns, MPI_UNSIGNED_CHAR, rank - 1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // printf("[%d]: Sending row %d to task %d\n", rank, initialRow, rank - 1);
            MPI_Send(auxBoard[1], columns, MPI_UNSIGNED_CHAR, rank - 1, tag, MPI_COMM_WORLD);
            if (rank < numtasks - 1) {
                // printf("[%d]: Sending row %d to task %d\n", rank, initialRow + numRows - 1, rank + 1);
                MPI_Send(auxBoard[numRows], columns, MPI_UNSIGNED_CHAR, rank + 1, tag, MPI_COMM_WORLD);
                // printf("[%d]: Receiving row %d from task %d\n", rank, initialRow + numRows, rank + 1);
                MPI_Recv(auxBoard[numRows + 1], columns, MPI_UNSIGNED_CHAR, rank + 1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            for (i = 0; i < numRows + padding; ++i) {
                for (j = 0; j < columns; ++j) {
                    board[i][j] = auxBoard[i][j];
                }
            }
        }
    }

    // Agregate
    if (rank == 0) {
        for (i = 1; i < numtasks; ++i) {
            unsigned int initialRow = taskGetInitialRow(i, numtasks, rows);
            unsigned int numRows = taskGetNumRows(i, numtasks, rows);
            for (j = 0; j < numRows; ++j) {
                MPI_Recv(board[initialRow + j], columns, MPI_UNSIGNED_CHAR, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
    }
    else {
        unsigned int numRows = taskGetNumRows(rank, numtasks, rows);
        for (i = 1; i < numRows + 1; ++i) {
            MPI_Send(board[i], columns, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD);
        }
    }

    // Finalize
    if (rank == 0) {
        if (boardToFile(board, rows, columns, outputFile) != 0) {
            printf("Error writing output file: %s\n", outputFile);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        boardDestroy(board, rows, columns);
        boardDestroy(auxBoard, rows, columns);
    }
    else {
        unsigned int numRows = taskGetNumRows(rank, numtasks, rows);
        unsigned int padding = (rank < numtasks - 1) ? 2 : 1;
        boardDestroy(board, numRows + padding, columns);
        boardDestroy(auxBoard, numRows + padding, columns);
    }

    MPI_Finalize();

    return 0;
}