#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <mpi.h>

#define DATASIZE   64
#define WRITER_ID   0

void single_writer(int, int *, int, int, int *);
void multy_writer(int my_id, int *localvector, int localsize);
void remove_spaces(const char *input, char *result);
void mpiio_writer(int my_id, int *localvector, int localsize, int *istart);


int main(int argc, char *argv[])
{
    int my_id, ntasks, i, localsize;
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
    {
      istart[i] = i*localsize;
      printf("istart[%d] = %d\n", i, istart[i]);
    }

    for (i = 0; i < localsize; i++) {
        localvector[i] = i + 1 + localsize * my_id;
    }

    single_writer(my_id, localvector, localsize, ntasks, istart);

    multy_writer(my_id, localvector, localsize);
    mpiio_writer(my_id, localvector, localsize, istart);
    free(localvector);

    MPI_Finalize();

    return 0;
}

void single_writer(int my_id, int *localvector, int localsize, 
    int ntask, int *istart)
{
    int *fullvector;
    int i;
    int sendtag=0, recvtag=0;
    MPI_Status status;

    /* TODO: Implement a function that will write the data to file so that
       a single process does the file io. Use rank WRITER_ID as the io rank */

    if (my_id == WRITER_ID)
    {
      fullvector = malloc(DATASIZE*sizeof(int));
      memcpy(&fullvector[istart[WRITER_ID]], localvector, localsize*sizeof(int));
      if (ntask>1)
      {
        for (i=1; i< ntask; i++)
        {
          MPI_Recv(&fullvector[istart[i]], localsize, MPI_INT, i, 
              recvtag, MPI_COMM_WORLD, &status);
        }
      }


      FILE *f = fopen("posix_out.txt", "w");
      if (f == NULL)
      {
        printf("Error opening file!\n");
        exit(1);
      }
      
      for (i=0; i < DATASIZE; i++) 
        fprintf(f, "%d  ", fullvector[i]);
      fprintf(f, "\n");

      
      fclose(f);
      free(fullvector);
    }
    else
    {
      MPI_Send(localvector, localsize, MPI_INT, 0, sendtag, MPI_COMM_WORLD);
    }
}

void multy_writer(int my_id, int *localvector, int localsize)
{
    int i;
    char fname[250], trim[250];

    sprintf(fname, "posix_out_proc_%d.txt", my_id);
    remove_spaces(fname, trim);
    FILE *f = fopen(trim, "w");
    if (f == NULL)
    {
      printf("Error opening file!\n");
      exit(1);
    }
    
    for (i=0; i < localsize; i++) 
      fprintf(f, "%d  ", localvector[i]);
    fprintf(f, "\n");

    
    fclose(f);
}


void mpiio_writer(int my_id, int *localvector, int localsize, int *istart)
{
    int i;
    MPI_File fh;
    MPI_Offset offset;
    MPI_Status status;

    offset = istart[my_id]*sizeof(int);

    MPI_File_open(MPI_COMM_WORLD, "mpi_io.txt", MPI_MODE_WRONLY | MPI_MODE_CREATE,
        MPI_INFO_NULL, &fh);
    
    MPI_File_write_at(fh, offset, localvector, localsize,
        MPI_INT, &status);
    
    MPI_File_close(&fh);
}

void remove_spaces(const char *input, char *result)
{
  int i, j = 0;
  for (i = 0; input[i] != '\0'; i++) {
    if (!isspace((unsigned char) input[i])) {
      result[j++] = input[i];
    }
  }
  result[j] = '\0';
}
