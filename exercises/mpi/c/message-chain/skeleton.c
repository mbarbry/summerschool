#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>


int main(int argc, char *argv[])
{
    int i, myid, ntasks;
    //int size = 10000000;
    int size = 10;
    int *message;
    int *receiveBuffer;
    int sendtag = 0,recvtag = 0;
    MPI_Status status;

    double t0, t1;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    /* Allocate message */
    message = malloc(sizeof(int) * size);
    receiveBuffer = malloc(sizeof(int) * size);
    /* Initialize message */
    for (i = 0; i < size; i++)
        message[i] = myid;

    /* Start measuring the time spend in communication */
    MPI_Barrier(MPI_COMM_WORLD);
    t0 = MPI_Wtime();

    printf("start sendRecv\n");
    /* TODO start */
    /* Send and receive messages as defined in exercise */
    if (ntasks > 1)
    {
      if (myid == 0)
      {
          MPI_Send(&message, size, MPI_INT, myid+1, sendtag,
              MPI_COMM_WORLD);
          MPI_Recv(&receiveBuffer, size, MPI_INT, ntasks-1, recvtag,
              MPI_COMM_WORLD, &status);
          printf("Sender: %d. Sent elements: %d. Tag: %d. Receiver: %d\n",
                 myid, size, myid + 1, myid + 1);
      }
      else if (myid == ntasks-1)
      {
          MPI_Send(&message, size, MPI_INT, 0, sendtag,
              MPI_COMM_WORLD);
          MPI_Recv(&receiveBuffer, size, MPI_INT, myid-1, recvtag,
              MPI_COMM_WORLD, &status);
          printf("Sender: %d. Sent elements: %d. Tag: %d. Receiver: %d\n",
                 myid, size, myid + 1, myid + 1);
      }
      else {
          printf("Hola rank %d\n", myid);

          MPI_Send(&message, size, MPI_INT, myid+1, sendtag,
              MPI_COMM_WORLD);
          MPI_Recv(&receiveBuffer, size, MPI_INT, myid-1, recvtag,
              MPI_COMM_WORLD, &status);
          printf("Sender: %d. Sent elements: %d. Tag: %d. Receiver: %d\n",
                 myid, size, myid + 1, myid + 1);
      }
    }
    printf("end sendRecv\n");

    if (myid > 0) {

        printf("Receiver: %d. first element %d.\n",
               myid, receiveBuffer[0]);
    }

    /* TODO end */

    /* Finalize measuring the time and print it out */
    t1 = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
    fflush(stdout);

    printf("Time elapsed in rank %2d: %6.3f\n", myid, t1 - t0);

    free(message);
    free(receiveBuffer);
    MPI_Finalize();
    return 0;
}
