#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>


#define PATHNAME1 "/home/trongdung/Device-Driver-Linux/OS_general/buoi6_multiThread/bai5/file1.txt"
#define PATHNAME2 "/home/trongdung/Device-Driver-Linux/OS_general/buoi6_multiThread/bai5/file2.txt"

static int i = 48;
sem_t semaphore;
pthread_t self;

void *thread_handle(void *arg)
{
    self = pthread_self();
    while(1){
        int fd = -1;
        if(-1 == sem_trywait(&semaphore)){
            if(-1 == (fd = open(PATHNAME2, O_RDWR | O_APPEND))){
                printf("Thread %ld Can not open file2\n",self);
                printf("Errno %s\n",strerror(errno));
                exit(-1);
            }   
        }
        else{
            if(-1 == (fd = open(PATHNAME1, O_RDWR | O_APPEND))){
                printf("Thread %ld Can not open file1\n",self);
                printf("Errno %s\n",strerror(errno));
                exit(-1);
            }
        }

        i++;
        if(i > 52)
        {
            i = 48;
        }
        write(fd,&i,1);
        close(fd);
        sem_post(&semaphore);
    }
    return 0;
}

void *t4_handle(void * arg)
{
    pthread_t *thread = (pthread_t*)arg;

    pthread_join(thread[0],NULL);
    pthread_join(thread[1],NULL);
    pthread_join(thread[2],NULL);
    pthread_join(thread[3],NULL);
    return 0;
}

int main()
{
    system("touch /home/trongdung/Device-Driver-Linux/OS_general/buoi6_multiThread/bai5/file1.txt");
    system("touch /home/trongdung/Device-Driver-Linux/OS_general/buoi6_multiThread/bai5/file2.txt");
    system("chmod 600 /home/trongdung/Device-Driver-Linux/OS_general/buoi6_multiThread/bai5/file1.txt");
    system("chmod 600 /home/trongdung/Device-Driver-Linux/OS_general/buoi6_multiThread/bai5/file2.txt");

    pthread_t t[4];
    pthread_create(&t[0],NULL,thread_handle,NULL);
    pthread_create(&t[1],NULL,thread_handle,NULL);
    pthread_create(&t[2],NULL,thread_handle,NULL);
    pthread_create(&t[3],NULL,t4_handle,t);

    sem_init(&semaphore,0,2);
    sleep(10000);
    sem_destroy(&semaphore);

    return 0;
}