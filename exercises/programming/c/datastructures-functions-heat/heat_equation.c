#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef _OPENMP
  #include <omp.h>
#endif

#ifdef MPI
  #include <mpi.h>
#endif

#include "pngwriter.h"
#include "laplacian.h"


int main(int argc, char *argv[])
{
  double alpha=0.5, dt=0.0;
  int nsteps = 500, it, i, j;
  int num_threads = 0;
  int istart = 0, iend = 0;
  char fname[255], trim[255];
  double fact1 = 0.0, fact2 = 0.0;
  arr_str arr;
  clock_t start_clock;

  #pragma omp parallel shared(num_threads)
  {
    #pragma omp master
#ifdef _OPENMP
    num_threads = omp_get_num_threads();
#else
    num_threads = 1;
#endif
  }

#ifdef MPI
  int rank = -1, nb_proc = 0, ip = 0;
  int pts_per_proc = 0, buffer_size = 0;
  int *arr_start;
  MPI_Status status;
  int sendtag = 0, recvtag = 0;

  MPI_Init(&argc, &argv);
#endif
 
  read_2D_array("bottle.dat", &arr);
#ifdef MPI
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  num_threads = nb_proc;

  arr_start = malloc(nb_proc*sizeof(int));
  printf("Hola MPI from process %d over %d\n", rank, nb_proc);

  pts_per_proc = (arr.nx + nb_proc - 1)/nb_proc;
  istart = rank*pts_per_proc;
  for (ip=0; ip < nb_proc; ip++)
  {
    arr_start[ip] = ip*pts_per_proc*arr.ny;
    printf("arrstart[%d] = %d\n", ip, arr_start[ip]);
  }

  int tmp = (rank+1)*pts_per_proc;
  //printf("pts_per_proc = %d, tmp = %d\n", pts_per_proc, tmp);
  iend = min_int(tmp, arr.nx);
  buffer_size = (iend - istart)*arr.ny;
  printf("iend = %d, istart = %d => buffer size: %d\n", iend, istart, buffer_size);
  //exit(1);
#else
  istart = 0;
  iend = arr.nx;
#endif
 
  double sum = sum_double(arr.input, arr.nx, arr.ny);
  printf("sum input = %lf\n", sum);

  arr.dx = 0.01;
  arr.dy = 0.01;
  arr.dx2 = arr.dx*arr.dx;
  arr.dy2 = arr.dy*arr.dy;
  dt = arr.dx2 * arr.dy2 / (2.0 * alpha * (arr.dx2 + arr.dy2));

  //arr.laplacian = malloc(arr.nx*arr.ny*sizeof(double));
  arr.temp = malloc(arr.nx*arr.ny*sizeof(double));

  /* Get the start time stamp */
  start_clock = clock();

  #pragma omp parallel shared(nsteps, dt, alpha, arr) \
    private(it, i, j, fact1, fact2)
  {
    for (it = 0; it < nsteps; it++)
    {
      //initialise_array(arr.laplacian, arr.nx, arr.ny, 0.0);
      //calc_laplacian(&arr);

      #pragma omp for
      for (i=istart; i<iend; i++)
      { 
        for (j=0; j<arr.ny; j++)
        { 
          fact1 = (arr.input[get_index(i-1, j, arr.nx, arr.ny)] - 
                  2.0*arr.input[get_index(i, j, arr.nx, arr.ny)] 
                  + arr.input[get_index(i+1, j, arr.nx, arr.ny)])/(arr.dx2);
          fact2 = (arr.input[get_index(i, j-1, arr.nx, arr.ny)] - 
                  2.0*arr.input[get_index(i, j, arr.nx, arr.ny)] 
                  + arr.input[get_index(i, j+1, arr.nx, arr.ny)])/(arr.dy2);
          arr.temp[get_index(i, j, arr.nx, arr.ny)] = 
            arr.input[get_index(i, j, arr.nx, arr.ny)]
            + dt*alpha*(fact1+fact2);
            //arr.laplacian[get_index(i, j, arr.nx, arr.ny)];
        }
      }
      //sum = sum_square_double(arr.temp, arr.nx, arr.ny);
      //printf("it = %d, proc = %d, sum temp = %lf\n", it, rank, sum);
      //if (it == 50) exit(1);

#ifdef MPI
      // Copy everythin into master proc
      if (rank > 0)
      {
        MPI_Send(&arr.temp[arr_start[rank]], buffer_size, MPI_DOUBLE, 0, sendtag, 
            MPI_COMM_WORLD);
      }
      else if (rank == 0)
      {
        for (ip = 1; ip < nb_proc; ip++)
        {
          MPI_Recv(&arr.temp[arr_start[ip]], buffer_size, MPI_DOUBLE, ip, recvtag, 
              MPI_COMM_WORLD, &status);
        }
        //sum = sum_square_double(arr.temp, arr.nx, arr.ny);
        //printf("it = %d, proc = %d, sum temp after recv= %lf\n", it, rank, sum);
      //MPI_Allgather();
      //if (rank > 0)
#endif
      #pragma omp master
      {
        sprintf(fname, "bottle_openmp/temp_step_%d.txt", it);
        remove_spaces(fname, trim);
        printf("outname: %s\n", trim);
        write_2D_array(arr.temp, arr.nx, arr.ny, trim);
        memcpy(arr.input, arr.temp, arr.nx*arr.ny*sizeof(double));
      }
#ifdef MPI
        for (ip = 1; ip < nb_proc; ip++)
        {
          MPI_Send(&arr.input[arr_start[ip]], buffer_size, MPI_DOUBLE, ip, sendtag, 
            MPI_COMM_WORLD);
        }
      }

      if (rank >0)
      {
        MPI_Recv(&arr.temp[arr_start[rank]], buffer_size, MPI_DOUBLE, 0, recvtag, 
              MPI_COMM_WORLD, &status);
      }
#endif
    }
  }

  /* Determine the CPU time used for the iteration */
  printf("Iteration took %.3f seconds with %d threads.\n", 
      (double)(clock() - start_clock) /(double)CLOCKS_PER_SEC, num_threads);

  printf("free\n");
  free(arr.input);
  free(arr.temp);
  free(arr_start);

  printf("end free\n");
#ifdef MPI
  MPI_Finalize();
  printf("end finalize\n");
#endif
  return 0;
}
