#include "wrapper.h"


int i = 48;

void my_write(char* file_path)
{
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex,NULL);
    pthread_mutex_lock(&mutex);
    
    FILE* fp = fopen(file_path,"wa");
    if(fp == NULL)
    {
        perror("can not open file\n");
        exit(-1);
    }

    i++;
    fwrite(i,sizeof(i),1,fp);
    fclose(fp);

    pthread_mutex_destroy(&mutex);
}