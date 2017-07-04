#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int rank;
    int dims[2] = {8, 8};
    int array[dims[0]][dims[1]];
    int sendtag, recvtag;
    MPI_Datatype coltype;
    MPI_Status status;

    int i, j;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Initialize arrays
    if (rank == 0) {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                array[i][j] = (i + 1) * 10 + j + 1;
            }
        }
    } else {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                array[i][j] = 0;
            }
        }
    }

    if (rank == 0) {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                printf("%3d", array[i][j]);
            }
            printf("\n");
        }
    }
 

    //TODO: Create datatype that describes one column. Use MPI_Type_vector.
    
    MPI_Type_vector(dims[0], 1, dims[1], MPI_INT, &coltype);
    MPI_Type_commit(&coltype);
    
    //TODO: Send first column of matrix form rank 0 to rank 1


    
    if (rank ==0) MPI_Send(array, 1, coltype, 1, sendtag, MPI_COMM_WORLD);
    if (rank ==1) MPI_Recv(array, 1, coltype, 0, recvtag, MPI_COMM_WORLD, &status);

    MPI_Type_free(&coltype);

    // Print out the result on rank 1
    // The application is correct if the first column has the values of rank 0
    if (rank == 1) {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                printf("%3d", array[i][j]);
            }
            printf("\n");
        }
    }
    
    MPI_Finalize();

    return 0;
}
