#include <stdio.h>
#include <stdlib.h>

void increment_ptrs(int *a, int *b);
int main (void)
{
  int a, b;
  a = 10;
  b = 20;

  increment_ptrs(&a, &b);

  printf("a = %d, b = %d\n", a, b);

  return 0;
}

void increment_ptrs(int *a, int *b)
{
  *a += 1;
  *b += 1;
}
