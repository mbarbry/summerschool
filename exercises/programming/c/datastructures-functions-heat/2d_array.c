#include <stdio.h>
#include <stdlib.h>

//#define NX 258
//#define NY 258
//


int get_index(int i, int j, int dimx, int dimy)
{
  return i*dimy + j;
}

void initialise_laplacian(double *arr, int NX, int NY)
{
    int i, j;
    
    // laplacian to zero
    for (i=0; i<NX; i++)
    {
      for (j=0; j<NY; j++)
      {
        arr[get_index(i, j, NX, NY)] = 0.0;
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
