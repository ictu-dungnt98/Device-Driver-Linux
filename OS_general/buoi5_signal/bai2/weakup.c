#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    system("gcc -o sleep sleep.c");
    
    int pid = fork();

    switch(pid)
    {
        case -1:
        {
            return -1;
        }
        case 0: // child
        {
            system("./sleep");
            break;
        }
        default: //parent
        {
            
            /* get pid of other process */
            char pidline[1024] = "";
            char* temp = NULL;
            FILE* pipe = popen("pidof sleep","r");
            if(pipe == NULL)
            {
                perror("Error open pile\n");
                exit(-1);
            }
            fgets(pidline,1024,pipe);
            temp = strtok(pidline," ");
            pid = atoi(temp);
            /************************************/
            printf("pid is gotten : %d\n",pid);

            kill(pid,12);
            
            pclose(pipe);
            break;
        }
    }   
    return 0;
}