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
  MPI_Status status;
  int sendtag = 0, recvtag = 0;
  double *halo_left, *halo_right;
  double *halo_left_recv, *halo_right_recv;

  MPI_Init(&argc, &argv);
 
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  num_threads = nb_proc;
  printf("Hola rank %d\n", rank);
  read_input("bottle.dat", &arr);
  printf("end input rank %d\n", rank);
  halo_left = malloc(arr.ny*sizeof(double));
  halo_right = malloc(arr.ny*sizeof(double));
  halo_left_recv = malloc(arr.ny*sizeof(double));
  halo_right_recv = malloc(arr.ny*sizeof(double));
#else
  read_input("bottle.dat", &arr);
  arr.istart = 0;
  arr.iend = arr.nx;
#endif
 
  //double sum = sum_double(arr.input, arr.nx, arr.ny);
  //printf("rank %d: sum input = %lf\n", rank, sum);

  arr.dx = 0.01;
  arr.dy = 0.01;
  arr.dx2 = arr.dx*arr.dx;
  arr.dy2 = arr.dy*arr.dy;
  dt = arr.dx2 * arr.dy2 / (2.0 * alpha * (arr.dx2 + arr.dy2));

  /* Get the start time stamp */
  start_clock = clock();

  #pragma omp parallel shared(nsteps, dt, alpha, arr) \
    private(it, i, j, fact1, fact2)
  {
    for (it = 0; it < nsteps; it++)
    {
      /*
      sprintf(fname, "bef_loop_temp_step_%d_rank_%d.txt", it, rank);
      remove_spaces(fname, trim);
      printf("outname: %s\n", trim);
      write_2D_array(arr.input, arr.nx-arr.istart, arr.ny, trim);
      */
      //printf("it = %d, rank = %d\n", it, rank);

      #pragma omp for
      for (i=arr.istart; i<arr.iend; i++)
      { 
#ifdef MPI
        /*
          printf("rank %d: get_index(arr.istart, 0, arr.nx, arr.ny) = %d, buff = %d\n",
              rank, get_index(arr.istart, 0, arr.nx, arr.ny), arr.buffer_size);
          printf("rank %d: get_index(arr.iend-1, 0, arr.nx, arr.ny) = %d, buff = %d\n",
              rank, get_index(arr.iend-1, 0, arr.nx, arr.ny), arr.buffer_size);
              */
          memcpy(halo_left, &arr.init_array[get_index(arr.istart, 0, arr.nx, arr.ny)], 
              arr.ny*sizeof(double));
          memcpy(halo_right, &arr.init_array[get_index(arr.iend-1, 0, arr.nx, arr.ny)], 
              arr.ny*sizeof(double));
        if (rank == 0)
        {
          MPI_Send(halo_right, arr.ny, MPI_DOUBLE, rank+1, sendtag, MPI_COMM_WORLD);
          MPI_Recv(halo_right_recv, arr.ny, MPI_DOUBLE, rank+1, recvtag, MPI_COMM_WORLD, &status);
        }
        else if (rank == nb_proc-1)
        {
          MPI_Send(halo_left, arr.ny, MPI_DOUBLE, rank-1, sendtag, MPI_COMM_WORLD);
          MPI_Recv(halo_left_recv, arr.ny, MPI_DOUBLE, rank-1, recvtag, MPI_COMM_WORLD, &status);
        }
        else
        {
          MPI_Send(halo_right, arr.ny, MPI_DOUBLE, rank+1, sendtag, MPI_COMM_WORLD);
          MPI_Recv(halo_right_recv, arr.ny, MPI_DOUBLE, rank+1, recvtag, MPI_COMM_WORLD, &status);
          MPI_Send(halo_left, arr.ny, MPI_DOUBLE, rank-1, sendtag, MPI_COMM_WORLD);
          MPI_Recv(halo_left_recv, arr.ny, MPI_DOUBLE, rank-1, recvtag, MPI_COMM_WORLD, &status);
        }
        //printf("i = %d, rank = %d\n", i, rank);
#endif
        for (j=0; j<arr.ny; j++)
        { 
#ifdef MPI
          if (i == 0 && rank >0)
          {
            fact1 = (halo_left[j] - 
                    2.0*arr.init_array[get_index(i, j, arr.nx, arr.ny)-arr.istart*arr.ny] 
                    + arr.init_array[get_index(i+1, j, arr.nx, arr.ny)-arr.istart*arr.ny])/(arr.dx2);
          }
          if (i == arr.nx-1 && rank <nb_proc-1)
          {
            fact1 = (arr.init_array[get_index(i-1, j, arr.nx, arr.ny)-arr.istart*arr.ny] - 
                    2.0*arr.init_array[get_index(i, j, arr.nx, arr.ny)-arr.istart*arr.ny] 
                    + halo_right[i])/(arr.dx2);
          }
          else 
          {
            fact1 = (arr.init_array[get_index(i-1, j, arr.nx, arr.ny)-arr.istart*arr.ny] - 
                    2.0*arr.init_array[get_index(i, j, arr.nx, arr.ny)-arr.istart*arr.ny] 
                    + arr.init_array[get_index(i+1, j, arr.nx, arr.ny)-arr.istart*arr.ny])/(arr.dx2);
          }
          fact2 = (arr.init_array[get_index(i, j-1, arr.nx, arr.ny)-arr.istart*arr.ny] - 
                  2.0*arr.init_array[get_index(i, j, arr.nx, arr.ny)-arr.istart*arr.ny] 
                  + arr.init_array[get_index(i, j+1, arr.nx, arr.ny)-arr.istart*arr.ny])/(arr.dy2);
          arr.temp[get_index(i, j, arr.nx, arr.ny)-arr.istart*arr.ny] = 
            arr.init_array[get_index(i, j, arr.nx, arr.ny)-arr.istart*arr.ny]
            + dt*alpha*(fact1+fact2);

#else
          fact1 = (arr.input[get_index(i-1, j, arr.nx, arr.ny)] - 
                  2.0*arr.input[get_index(i, j, arr.nx, arr.ny)] 
                  + arr.input[get_index(i+1, j, arr.nx, arr.ny)])/(arr.dx2);
 
          fact2 = (arr.input[get_index(i, j-1, arr.nx, arr.ny)] - 
                  2.0*arr.input[get_index(i, j, arr.nx, arr.ny)] 
                  + arr.input[get_index(i, j+1, arr.nx, arr.ny)])/(arr.dy2);
          arr.temp[get_index(i, j, arr.nx, arr.ny)] = 
            arr.input[get_index(i, j, arr.nx, arr.ny)]
            + dt*alpha*(fact1+fact2);
#endif
        }
      }
      //sum = sum_square_double(arr.temp, arr.nx, arr.ny);
      //printf("it = %d, proc = %d, sum temp = %lf\n", it, rank, sum);
#ifdef MPI
      /*
      sprintf(fname, "bef_gather_temp_step_%d_rank_%d.txt", it, rank);
      remove_spaces(fname, trim);
      printf("outname: %s\n", trim);
      write_2D_array(arr.temp, arr.nx-arr.istart, arr.ny, trim);
      */

      MPI_Gatherv(arr.temp, arr.buffer_size, MPI_DOUBLE, arr.input, 
          arr.recvcounts, arr.displ, MPI_DOUBLE, 0, MPI_COMM_WORLD);
      //MPI_Allgather(arr.temp, recvcounts[rank], MPI_DOUBLE, arr.input,
      //    recvcounts[rank], MPI_DOUBLE, MPI_COMM_WORLD);


      /*
      sprintf(fname, "temp_step_%d_rank_%d.txt", it, rank);
      remove_spaces(fname, trim);
      printf("outname: %s\n", trim);
      write_2D_array(arr.input, arr.nx, arr.ny, trim);

      if (it == 5) exit(1);
      */

      if (rank == 0)
      {
        sprintf(fname, "bottle_openmp/temp_step_%d.txt", it);
        remove_spaces(fname, trim);
        printf("outname: %s\n", trim);
        //memcpy(arr.input, arr.temp, arr.nx*arr.ny*sizeof(double));
        //printf("end memcpy\n");
        write_2D_array(arr.input, arr.nx, arr.ny, trim);
      }
      distribut_array(arr.input, arr.init_array, arr.buffer_size,
          rank, nb_proc, arr.displ);
#else
      #pragma omp master
      {
        sprintf(fname, "bottle_openmp/temp_step_%d.txt", it);
        remove_spaces(fname, trim);
        printf("outname: %s\n", trim);
        memcpy(arr.input, arr.temp, arr.nx*arr.ny*sizeof(double));
        write_2D_array(arr.input, arr.nx, arr.ny, trim);
      }
#endif
    }
  }

  /* Determine the CPU time used for the iteration */
  printf("Iteration took %.3f seconds with %d threads.\n", 
      (double)(clock() - start_clock) /(double)CLOCKS_PER_SEC, num_threads);

  printf("free\n");
  free_arr_str(&arr);
  printf("end free\n");
#ifdef MPI
  free(halo_left);
  free(halo_right);
  free(halo_left_recv);
  free(halo_right_recv);
  MPI_Finalize();
  printf("end finalize\n");
#endif
  return 0;
}
