#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _OPENMP
  #include <omp.h>
#endif

#ifdef MPI
  #include <mpi.h>
#endif

#include "laplacian.h"

int min_int(int a, int b)
{
  if (a<b)
  {
    return a;
  }
  else
  {
    return b;
  }
}

double sum_double(double *arr, int nx, int ny)
{

  double sum = 0.0;
  int i, j;

  for (i=0; i<nx; i++)
  {
    for (j=0; j<ny; j++)
    {
      sum += arr[get_index(i, j, nx, ny)];
    }
  }

  return sum;
}

double sum_square_double(double *arr, int nx, int ny)
{

  double sum = 0.0;
  int i, j;

  for (i=0; i<nx; i++)
  {
    for (j=0; j<ny; j++)
    {
      sum += arr[get_index(i, j, nx, ny)]*arr[get_index(i, j, nx, ny)];
    }
  }

  return sum;
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

int get_index(int i, int j, int dimx, int dimy)
{
  return i*dimy + j;
}

void initialise_array(double *arr, int NX, int NY, double factor)
{
    int i, j;
    
    // laplacian to zero
    #pragma omp parallel shared(arr, NX, NY, factor) private(i, j)
    {
      #pragma omp for
      for (i=0; i<NX; i++)
      {
        for (j=0; j<NY; j++)
        {
          arr[get_index(i, j, NX, NY)] = factor;
        }
      }
    }
}

void write_2D_array(double *arr, int NX, int NY, char *fname)
{
    int i, j;
    FILE *f = fopen(fname, "w");
    if (f == NULL)
    {
          printf("Error opening file!\n");
          exit(1);
    }
   
    // laplacian to zero
    for (i=0; i<NX; i++)
    {
      for (j=0; j<NY; j++)
      {
        fprintf(f, "%f  ", arr[get_index(i, j, NX, NY)]);
      }
      fprintf(f, "\n");
    }
    fclose(f);
}

void read_input(char *fname, arr_str *arr)
{
#ifdef MPI
    int rank = -1, nb_proc = 0;
    int pts_per_proc = 0, ip;
    int dimen[2];
    int sendtag=0, recvtag=0;
    MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;

    if (rank == 0)
    {
      read_file_array(fname, arr);
      dimen[0] = arr->nx;
      dimen[1] = arr->ny;

      for (ip = 1; ip < nb_proc; ip++)
      {
        MPI_Send(&dimen, 2, MPI_INT, ip, sendtag,
          MPI_COMM_WORLD);
      }
    }
    else if (rank >0)
    {
      MPI_Recv(&dimen, 2, MPI_INT, 0, recvtag,
        MPI_COMM_WORLD, &status);
      arr->nx = dimen[0];
      arr->ny = dimen[1];
    }

    pts_per_proc = (arr->nx + nb_proc - 1)/nb_proc;
    arr->istart = rank*pts_per_proc;
    arr->iend = min_int((rank+1)*pts_per_proc, arr->nx);
    arr->buffer_size = (arr->iend - arr->istart)*arr->ny;
    arr->init_array = malloc(arr->buffer_size*sizeof(double));
    arr->temp = malloc(arr->buffer_size*sizeof(double));
    printf("rank %d: arr->nx = %d, arr->ny =%d, buffer = %d\n", 
        rank, arr->nx, arr->ny, arr->buffer_size);

    arr->displ = malloc(nb_proc*sizeof(int));
    arr->recvcounts = malloc(nb_proc*sizeof(int));

    for (ip=0; ip < nb_proc; ip++)
    {
      arr->displ[ip] = ip*pts_per_proc*arr->ny;
      arr->recvcounts[ip] = (arr->iend - arr->istart)*arr->ny;
    }

    distribut_array(arr->input, arr->init_array, arr->buffer_size,
        rank, nb_proc, arr->displ);
    /*
    MPI_Sendrecv(&arr->input[arr->displ[rank]], arr->buffer_size, 
        MPI_DOUBLE, rank, sendtag, arr->init_array, arr->buffer_size,
        MPI_DOUBLE, 0, recvtag, MPI_COMM_WORLD, &status);
    */
    printf("end sendrecv input, rank %d!\n", rank);
#else
    read_file_array(fname, arr);
    arr->temp = malloc(arr->nx*arr->ny*sizeof(double));
    arr->init_array = arr->input;
#endif
}

void free_arr_str(arr_str *arr)
{
  free(arr->input);
  free(arr->temp);
#ifdef MPI
  free(arr->displ);
  free(arr->recvcounts);
  free(arr->init_array);
#endif
}

void distribut_array(double *arr_send, double *arr_recv, int buffer_size,
    int rank, int nb_proc, int *displ)
{
#ifdef MPI
  int sendtag, recvtag;
  int ip;
  MPI_Status status;

  sendtag = 0; recvtag = 0;
  // share the input to the different process
  if (rank == 0)
  {
    memcpy(arr_recv, &arr_send[displ[rank]], 
        buffer_size*sizeof(double));
    for (ip = 1; ip < nb_proc; ip++)
      MPI_Send(&arr_send[displ[ip]], buffer_size,
          MPI_DOUBLE, ip, sendtag, MPI_COMM_WORLD);
  }
  else
  {
    MPI_Recv(arr_recv, buffer_size, MPI_DOUBLE, 0, recvtag,
        MPI_COMM_WORLD, &status);
  }
#endif
}

void read_file_array(char *fname, arr_str *arr)
{
  int i, j;
  double num;
  FILE *f = fopen(fname, "r");

  if (f == NULL)
  {
        printf("Error opening file!\n");
        exit(1);
  }

  fscanf(f, "%d  %d\n", &arr->nx, &arr->ny);
  arr->input = malloc(arr->nx*arr->ny*sizeof(double));
  num = 0.0;

  for (i=0; i<arr->nx; i++)
  {
    for (j=0; j<arr->ny; j++)
    {
      fscanf(f, "%lf  ", &num);
      arr->input[get_index(i, j, arr->nx, arr->ny)] = num;
    }
  }
  fclose(f);

}

void initialise_input_array(double *arr, int NX, int NY)
{
    int i, j;
    
    for (i=0; i<NX; i++)
    {
      for (j=0; j<NY; j++)
      {
        if (j == 0)
        {
          arr[get_index(i, j, NX, NY)] = 85.0;
        }
        else if (j == NY-1)
        {
          arr[get_index(i, j, NX, NY)] = 5.0;
        }
        else if (i == 0)
        {
          arr[get_index(i, j, NX, NY)] = 20.0;
        }
        else if (i == NX-1)
        {
          arr[get_index(i, j, NX, NY)] = 70.0;
        }
        else
        {
          arr[get_index(i, j, NX, NY)] = 0.0;
        }
      }
    }

}
