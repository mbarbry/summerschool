typedef struct {
  int nx, ny;
  double dx, dy, dx2, dy2;
  double *input, *laplacian;
} arr_str;

void initialise_input_array(double *arr, int NX, int NY);
void initialise_laplacian(double *arr, int NX, int NY);
int get_index(int i, int j, int dimx, int dimy);
void write_2D_array(double *arr, int NX, int NY, char *fname);
void calc_laplacian(arr_str arr);
