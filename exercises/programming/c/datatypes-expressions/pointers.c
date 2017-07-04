#include <stdio.h>

int main(int argc, char *argv[])
{
   int a, b, c;
   int *p;
   // declare a pointer variable and assign it to address of e.g. a

   a = 5;
   b = 2;
   c = 3;
   
   p = &a;
   printf("value pointing by p: %d\n", *p);
   // Evaluate expressions using both the original and the pointer variable
   // and investigate the value / value pointed to
   
   
   //print pointer address
   printf("pointer address %p\n", p);

   printf("a+b+c = %d\n", a+b+c);
   printf("p+b+c = %d\n", *p+b+c);



   return 0;
}
