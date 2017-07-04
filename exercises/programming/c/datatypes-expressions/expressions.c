#include <stdio.h>

int main(int argc, char *argv[])
{
   // declare integers, floats, and doubles
   double a = 2;
   int b = 3.0;
   float c = 0.0;


   // evaluate expressions, e.g. c = a + 2.5 * b
   // and print out results. Try to combine integers, floats
   // and doubles and investigate what happens in type conversions

   c = a+2.5*b;
   printf("c = %f\n", c);

   return 0;
}
