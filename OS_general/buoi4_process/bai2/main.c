#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
int main()
{
    int var = 0;
    
    int pid = -1;
    pid = fork();
    int status = 0;
    switch(pid)
    {
        case -1:
        {
            perror("Error generate child process\n");
            exit(-1);
        }
        case 0: // child process
        {
            printf("I'm child process with pid = %d\n",getpid());
            system("gcc -o child child.c");
            if(-1 == execl("./child","child","./file.txt",NULL))
            {
                perror("Error when call child process\n");
                return -2;
            }
            break;
        }
        default:
        {
            printf("I'm parent process with pid = %d, ",getpid());
            printf("My son is %d\n",pid);
            wait(&status);
            printf("Child return to %d\n",status);
            break;
        }
    }
    return 0;
}