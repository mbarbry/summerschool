#include <stdio.h>
#include <stdlib.h>
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

void read_2D_array(char *fname, arr_str *arr)
{
    int i, j;
    double num;
    int istart, iend;
    FILE *f = fopen(fname, "r");

    if (f == NULL)
    {
          printf("Error opening file!\n");
          exit(1);
    }

    fscanf(f, "%d  %d\n", &arr->nx, &arr->ny);
    arr->input = malloc(arr->nx*arr->ny*sizeof(double));


    num = 0.0;
#ifdef MPI
    int rank = -1, nb_proc = 0;
    int pts_per_proc = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    pts_per_proc = (arr->nx + nb_proc - 1)/nb_proc;
    istart = rank*pts_per_proc;
    iend = min_int((rank+1)*pts_per_proc, arr->nx);
#else
    istart = 0;
    iend = arr->nx;
#endif
    for (i=istart; i<iend; i++)
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
