#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>


static unsigned long long i = 0;

static void * thread1_handle(void *arg)
{
    int j;
    for(j=0; j<25000000;j++)
    {
        if(j % 2 != 0)
        {
            i++;
        }
    }
}
static void * thread2_handle(void *arg)
{
    int j;
    for(j=25000000; j<50000000;j++)
    {
        if(j % 2 != 0)
        {
            i++;
        }
    }
}
static void * thread3_handle(void *arg)
{
    int j;
    for(j=50000000; j<75000000;j++)
    {
        if(j % 2 != 0)
        {
            i++;
        }
    }
}
static void * thread4_handle(void *arg)
{
    int j;
    for(j=75000000; j<100000000;j++)
    {
        if(j % 2 != 0)
        {
            i++;
        }
    }
}


int main()
{
    pthread_t thread1, thread2,thread3,thread4,thread5,thread6,thread7,thread8;
    clock_t start, end;

    if(0 != pthread_create(&thread1,NULL,thread1_handle, NULL))
    {
        perror("Can not create thread 1\n");
        exit(-1);
    }
    if(0 != pthread_create(&thread2,NULL,thread2_handle, NULL))
    {
        perror("Can not create thread 2\n");
      
    }
    if(0 != pthread_create(&thread3,NULL,thread3_handle, NULL))
    {
        perror("Can not create thread 1\n");
        exit(-1);
    }
    if(0 != pthread_create(&thread4,NULL,thread4_handle, NULL))
    {
        perror("Can not create thread 2\n");
      
    }
    
    start = clock();
    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    pthread_join(thread3,NULL);
    pthread_join(thread4,NULL);
    end = clock();

    printf("i = %lld\n",i);
    double time_taken = ((double)(end - start))/CLOCKS_PER_SEC;
    printf("time taken : %f\n", time_taken);

    return 0;
}