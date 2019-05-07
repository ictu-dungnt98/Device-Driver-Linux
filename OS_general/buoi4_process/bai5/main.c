#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
int main()
{
    int len = 0;
    char buff[1000];
    int fd;
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
            if( -1 == system("gcc -o child child.c"))
            {
                perror("Die when run system command\n");
                return -1;
            }
            execl("./child","child",NULL);
            break;
        }
        default:
        {
            printf("I'm parent process with pid = %d, ",getpid());
            printf("My son is %d\n",pid);
            wait(&status);
            printf("Child return to %d\n",status);
            if(status == 0)
            {
                fd = open("./file.txt",O_RDONLY);
                if(fd == -1)
                {
                    perror("Die when oepen file\n");
                    return -1;
                }
                while((len = read(fd,buff,sizeof(buff)) != 0))
                {
                    if(len == -1)
                    {
                        return -1;
                    }
                    printf("%s",buff);
                }
            }
            break;
        }
    }
    return 0;
}