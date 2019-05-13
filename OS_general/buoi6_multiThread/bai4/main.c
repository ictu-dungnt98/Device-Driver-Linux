#include "wrapper.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>



int i = 48;

void my_write(char* file_path)
{
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex,NULL);
    pthread_mutex_lock(&mutex);
    
    FILE* fp = fopen(file_path,"a");
    if(fp == NULL)
    {
        perror("can not open file\n");
        exit(-1);
    }

    i++;
    fwrite(&i,1,1,fp);
    fclose(fp);

    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
}

void* thread_handle(void* arg)
{
    my_write((char*)arg);
    return 0;
}


int main()
{
    pthread_t thread1,thread2;
    pthread_create(&thread1,NULL,thread_handle,"./file.txt");
    pthread_create(&thread2,NULL,thread_handle,"./file.txt");

    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    return 0;
}