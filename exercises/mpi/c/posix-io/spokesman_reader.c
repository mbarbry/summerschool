#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mpi.h>

#define DATASIZE   64
#define WRITER_ID   0

void single_reader(int, int *, int, int, int *);
void ordered_print(int, int, int *, int);

int main(int argc, char *argv[])
{
    int my_id, ntasks, localsize, i;
    int *localvector, *istart;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

    if (ntasks > 64) {
        fprintf(stderr, "Datasize (64) should be divisible by number "
                "of tasks.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    if (DATASIZE % ntasks != 0) {
        fprintf(stderr, "Datasize (64) should be divisible by number "
                "of tasks.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    localsize = DATASIZE / ntasks;
    localvector = (int *) malloc(localsize * sizeof(int));
    istart = (int *) malloc(ntasks * sizeof(int));
    
    for (i =0; i < ntasks; i++)
      istart[i] = i*localsize;

    single_reader(my_id, localvector, localsize, ntasks, istart);

    ordered_print(ntasks, my_id, localvector, localsize);

    free(localvector);
    free(istart);

    MPI_Finalize();
    return 0;
}

void single_reader(int my_id, int *localvector, int localsize, int ntask, 
    int *istart)
{
    FILE *fp;
    int *fullvector, nread;
    int sendtag=0, recvtag = 0, i;
    char *fname = "posix_out.txt";
    MPI_Status status;

    /* TODO: Implement a function that will read the data from a file so that
       a single process does the file io. Use rank WRITER_ID as the io rank */

    if (my_id == WRITER_ID)
    {
      fullvector = malloc(DATASIZE*sizeof(int));
      FILE *f = fopen(fname, "r");
      
      for (i = 0; i < DATASIZE; i++)
      {
        fscanf(f, "%d  ", &fullvector[i]);
      }
      
      fclose(f);

      memcpy(localvector, &fullvector[istart[WRITER_ID]], localsize*sizeof(int));
      if (ntask>1)
      {
        for (i=1; i< ntask; i++)
        {
          MPI_Send(&fullvector[istart[i]], localsize, MPI_INT, i, sendtag, MPI_COMM_WORLD);
        }
      }
      free(fullvector);
    }
    else
    {
      MPI_Recv(localvector, localsize, MPI_INT, 0,
          recvtag, MPI_COMM_WORLD, &status);
    }

}

/* Try to avoid this type of pattern when ever possible.
   Here we are using this serialized output just to make the
   debugging easier. */
void ordered_print(int ntasks, int rank, int *buffer, int n)
{
    int task, i;

    for (task = 0; task < ntasks; task++) {
        if (rank == task) {
            printf("Task %i received:", rank);
            for (i = 0; i < n; i++) {
                printf(" %2i", buffer[i]);
            }
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}
