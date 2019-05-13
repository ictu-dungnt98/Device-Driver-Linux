#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
int main()
{
    clock_t start, end;
    unsigned long long i;
    start = clock();
    char file[20] = "./non_file";
    int arr[100];
    int j;
    int fd;
    for(i = 0; i< 10; i++)
    {

        for(j=0; j<100;j++)
        {
            arr[j] = rand() % 10;
        }
        file[strlen(file)] = i+48;

        if(0 > (fd = open(file,O_RDWR | O_CREAT, S_IWUSR|S_IWOTH)))
        {
            perror("Can not open file\n");
            exit(-1);
        }
        if(fd < 0)
        {
            perror("Create file error\n");
            exit(-1);
        }
        write(fd,arr,sizeof(arr));
        close(fd);
    }
    end = clock();
    double time_taken = ((double)(end - start))/CLOCKS_PER_SEC;

    printf("time taken : %f\n", time_taken);

    return 0;
}