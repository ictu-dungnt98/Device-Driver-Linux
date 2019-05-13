#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>


static void * thread1_handle(void *arg)
{

    int i;
    int arr[100];
    for(i=0; i<100;i++)
    {
        arr[i] = rand() % 10;
    }

    int fd = open("./file1",O_RDWR | O_CREAT, S_IWUSR|S_IWOTH);
    if(fd < 0)
    {
        perror("Create file error\n");
        exit(-1);
    }
    write(fd,arr,sizeof(arr));
    close(fd);

    return 0;
}
static void * thread2_handle(void *arg)
{
    int i;
    int arr[100];
    for(i=0; i<100;i++)
    {
        arr[i] = rand() % 10;
    }

    int fd = open("./file2",O_RDWR | O_CREAT, S_IWUSR|S_IWOTH);
    if(fd < 0)
    {
        perror("Create file error\n");
        exit(-1);
    }
    write(fd,arr,sizeof(arr));
    close(fd);

    return 0;
}
static void * thread3_handle(void *arg)
{
    int i;
    int arr[100];
    for(i=0; i<100;i++)
    {
        arr[i] = rand() % 10;
    }

    int fd = open("./file3",O_RDWR | O_CREAT, S_IWUSR|S_IWOTH);
    if(fd < 0)
    {
        perror("Create file error\n");
        exit(-1);
    }
    write(fd,arr,sizeof(arr));
    close(fd);

    return 0;
}
static void * thread4_handle(void *arg)
{
    int i;
    int arr[100];
    for(i=0; i<100;i++)
    {
        arr[i] = rand() % 10;
    }

    int fd = open("./file4",O_RDWR | O_CREAT, S_IWUSR|S_IWOTH);
    if(fd < 0)
    {
        perror("Create file error\n");
        exit(-1);
    }
    write(fd,arr,sizeof(arr));
    close(fd);

    return 0;
}
static void * thread5_handle(void *arg)
{
    int i;
    int arr[100];
    for(i=0; i<100;i++)
    {
        arr[i] = rand() % 10;
    }

    int fd = open("./file5",O_RDWR | O_CREAT, S_IWUSR|S_IWOTH);
    if(fd < 0)
    {
        perror("Create file error\n");
        exit(-1);
    }
    write(fd,arr,sizeof(arr));
    close(fd);

    return 0;
}
static void * thread6_handle(void *arg)
{
        int i;
    int arr[100];
    for(i=0; i<100;i++)
    {
        arr[i] = rand() % 10;
    }

    int fd = open("./file6",O_RDWR | O_CREAT, S_IWUSR|S_IWOTH);
    if(fd < 0)
    {
        perror("Create file error\n");
        exit(-1);
    }
    write(fd,arr,sizeof(arr));
    close(fd);

    return 0;
}
static void * thread7_handle(void *arg)
{
    int i;
    int arr[100];
    for(i=0; i<100;i++)
    {
        arr[i] = rand() % 10;
    }

    int fd = open("./file7",O_RDWR | O_CREAT, S_IWUSR|S_IWOTH);
    if(fd < 0)
    {
        perror("Create file error\n");
        exit(-1);
    }
    write(fd,arr,sizeof(arr));
    close(fd);

    return 0;
}
static void * thread8_handle(void *arg)
{
    int i;
    int arr[100];
    for(i=0; i<100;i++)
    {
        arr[i] = rand() % 10;
    }

    int fd = open("./file8",O_RDWR | O_CREAT, S_IWUSR|S_IWOTH);
    if(fd < 0)
    {
        perror("Create file error\n");
        exit(-1);
    }
    write(fd,arr,sizeof(arr));
    close(fd);

    return 0;
}
static void * thread9_handle(void *arg)
{
    int i;
    int arr[100];
    for(i=0; i<100;i++)
    {
        arr[i] = rand() % 10;
    }

    int fd = open("./file9",O_RDWR | O_CREAT, S_IWUSR|S_IWOTH);
    if(fd < 0)
    {
        perror("Create file error\n");
        exit(-1);
    }
    write(fd,arr,sizeof(arr));
    close(fd);

    return 0;
}
static void * thread10_handle(void *arg)
{
    int i;
    int arr[100];
    for(i=0; i<100;i++)
    {
        arr[i] = rand() % 10;
    }

    int fd = open("./file10",O_RDWR | O_CREAT, S_IWUSR|S_IWOTH);
    if(fd < 0)
    {
        perror("Create file error\n");
        exit(-1);
    }
    write(fd,arr,sizeof(arr));
    close(fd);

    return 0;
}

int main()
{
    pthread_t thread1, thread2,thread3,thread4,thread5,thread6,thread7,thread8,thread9,thread10;
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
    if(0 != pthread_create(&thread5,NULL,thread5_handle, NULL))
    {
        perror("Can not create thread 1\n");
        exit(-1);
    }
    if(0 != pthread_create(&thread6,NULL,thread6_handle, NULL))
    {
        perror("Can not create thread 2\n");
      
    }
    if(0 != pthread_create(&thread7,NULL,thread7_handle, NULL))
    {
        perror("Can not create thread 1\n");
        exit(-1);
    }
    if(0 != pthread_create(&thread8,NULL,thread8_handle, NULL))
    {
        perror("Can not create thread 2\n");
      
    }
    if(0 != pthread_create(&thread9,NULL,thread9_handle, NULL))
    {
        perror("Can not create thread 1\n");
        exit(-1);
    }
    if(0 != pthread_create(&thread10,NULL,thread10_handle, NULL))
    {
        perror("Can not create thread 2\n");
      
    }
    
    start = clock();
    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    pthread_join(thread3,NULL);
    pthread_join(thread4,NULL);
    pthread_join(thread5,NULL);
    pthread_join(thread6,NULL);
    pthread_join(thread7,NULL);
    pthread_join(thread8,NULL);
    pthread_join(thread9,NULL);
    pthread_join(thread10,NULL);
    
    end = clock();

    double time_taken = ((double)(end - start))/CLOCKS_PER_SEC;
    printf("time taken : %f\n", time_taken);

    return 0;
}