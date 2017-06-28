#include <stdlib.h>
#include <stdio.h>
#include "pngwriter.h"
#include "laplacian.h"

int main(void)
{
  arr_str arr;
  int dimx = 256, dimy = 256;
  int error_code = 0;

  arr.nx = dimx+2;
  arr.ny = dimy+2;
  arr.dx = 0.01;
  arr.dy = 0.01;
  arr.dx2 = arr.dx*arr.dx;
  arr.dy2 = arr.dy*arr.dy;

  arr.input = malloc(arr.nx*arr.ny*sizeof(double));
  arr.laplacian = malloc(arr.nx*arr.ny*sizeof(double));

  initialise_input_array(arr.input, arr.nx, arr.ny);
  initialise_laplacian(arr.laplacian, arr.nx, arr.ny);

  calc_laplacian(arr);

  write_2D_array(arr.input, arr.nx, arr.ny, "input.txt");
  write_2D_array(arr.laplacian, arr.nx, arr.ny, "laplacian.txt");

  free(arr.input);
  free(arr.laplacian);

  return 0;
}
