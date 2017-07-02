typedef struct {
  int nx, ny;
  double dx, dy, dx2, dy2;
  double *input, *temp;
} arr_str;

int min_int(int a, int b);
double sum_double(double *arr, int nx, int ny);
double sum_square_double(double *arr, int nx, int ny);
void initialise_input_array(double *arr, int NX, int NY);
void initialise_array(double *arr, int NX, int NY, double factor);
int get_index(int i, int j, int dimx, int dimy);
void write_2D_array(double *arr, int NX, int NY, char *fname);
void read_2D_array(char *fname, arr_str *arr);
void remove_spaces(const char *input, char *result);
void calc_laplacian(arr_str *arr);
