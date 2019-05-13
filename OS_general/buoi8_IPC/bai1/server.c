#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define PAGE_SIZE   1024

int main()
{
    char* letter = NULL ;
    char preLetter[1024] = "";

    system("touch strtogenkey.txt");
    key_t key = ftok("strtogenkey.txt",65);
    if(key < 0)
    {
        perror("Can not cread key for shared memory\n");
        exit(-1);
    }

    int shm_identified = shmget(key,PAGE_SIZE,IPC_CREAT | 0666);
    if(shm_identified < 0)
    {
        perror("can not create new shm identifier\n");
        exit(-1);
    }

    letter = (char*)shmat(shm_identified,NULL,0); /* 0 is read and write */
    if(letter < 0)
    {
        perror("can not attach shared memory to process address space\n");
        exit(-1);
    }

    while(1)
    {
        printf("Enter your word to send to client: ");
        memset(letter,0,PAGE_SIZE);
        fgets(letter,1024,stdin);

        strcpy(preLetter,letter);
        while((0 == strcmp(preLetter,letter)) || (strlen(letter)==0)); /* Listen client connect */

        printf("client send: ");
        printf("%s",letter);
    }

    shmdt(letter);
    shmctl(shm_identified,IPC_RMID,NULL);
}