#include <stdio.h>
#ifdef _OPENMP
  #include <omp.h>
#endif

#define NX 1002400

int main(void)
{
    long vecA[NX];
    long sum, psum, sumex;
    int i, tid, nthreads;

    /* Initialization of the vectors */
    for (i = 0; i < NX; i++) {
        vecA[i] = (long) i+1;
    }

#ifdef _OPENMP
    #pragma omp parallel private(tid)
    {

      /* Obtain and print thread id */
      tid = omp_get_thread_num();
      printf("Hello World from thread = %d\n", tid);
      
      if (tid == 0) 
      {
        nthreads = omp_get_num_threads();
        printf("Number of threads = %d\n", nthreads);
      }
    }
#endif
    /* All threads join master thread and terminate */

    //Reduction Version
    sum = 0;
    /* TODO: Parallelize computation */
    #pragma omp parallel shared(vecA) private(i) \
      reduction(+: sum)
    {
      #pragma omp for
      for (i = 0; i < NX; i++) {
          sum += vecA[i];
      }
#ifdef _OPENMP
      printf("sum for thread %d: %ld\n", omp_get_thread_num(), sum);
#endif
    }

    //Critical Version
    sumex = 0;
    /* TODO: Parallelize computation */
    #pragma omp parallel shared(vecA, sumex) private(i, psum)
    {
      psum = 0;
      #pragma omp for
      for (i = 0; i < NX; i++) {
          psum += vecA[i];
      }
#ifdef _OPENMP
      printf("sum for thread %d: %ld\n", omp_get_thread_num(), psum);
#endif
      #pragma omp critical(addup)
      sumex += psum;
    }

    printf("Reduction: Sum: %ld\n",sum);
    printf("Critical: Sum: %ld\n",sumex);

    return 0;
}
