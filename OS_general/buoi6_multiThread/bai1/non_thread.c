#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    clock_t start, end;
    unsigned long long i;
    start = clock();
    for(i = 0; i< 100000; i++)
    {

    }
    end = clock();
    double time_taken = ((double)(end - start))/CLOCKS_PER_SEC;

    printf("time taken : %f\n", time_taken);

    return 0;
}