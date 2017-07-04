#include <stdio.h>

int main(int argc, char *argv[])
{
   long f0=0, f1=1, f2;
   long i =0;
   // initialize variables and write the while loop
   // Remember to update all variables within the loop

   while (i<100)
   {
     switch (i)
     {
       case (0):
         printf("f%ld = %ld\n", i, f0);
         break;
       case (1):
         printf("f%ld = %ld\n", i, f1);
         break;
       default:
         f2 = f1 + f0;
         printf("f%ld = %ld\n", i, f2);
         f0 = f1;
         f1 = f2;
         break;
     }
     i++;
   }

   return 0;
}
