#include <stdio.h>
#include <pthread.h>

int i = 0;

void* thread_handle(void * arg)
{
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex,NULL);

    pthread_mutex_lock(&mutex);
    i++;
    printf("%d\n",i);

    pthread_mutex_unlock(&mutex);

    return 0;
}

int main()
{
    pthread_t thread1, thread2;
    pthread_create(&thread1,NULL,thread_handle,NULL);
    pthread_create(&thread2,NULL,thread_handle,NULL);
    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    return 0;
}