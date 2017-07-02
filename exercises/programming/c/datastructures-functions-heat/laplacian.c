#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "laplacian.h"

void calc_laplacian(arr_str *arr)
{
    int i, j;
    double fact1, fact2;

    // Evaluate the Laplacian
    #pragma omp for
    for (i=1; i<arr->nx-1; i++)
    {
      for (j=1; j<arr->ny-1; j++)
      {
        fact1 = (arr->input[get_index(i-1, j, arr->nx, arr->ny)] - 
            2.0*arr->input[get_index(i, j, arr->nx, arr->ny)] 
            + arr->input[get_index(i+1, j, arr->nx, arr->ny)])/(arr->dx2);

        fact2 = (arr->input[get_index(i, j-1, arr->nx, arr->ny)] - 
            2.0*arr->input[get_index(i, j, arr->nx, arr->ny)] 
            + arr->input[get_index(i, j+1, arr->nx, arr->ny)])/(arr->dy2);

        //arr->laplacian[get_index(i, j, arr->nx, arr->ny)] = fact1 + fact2;
      }
    }
}
