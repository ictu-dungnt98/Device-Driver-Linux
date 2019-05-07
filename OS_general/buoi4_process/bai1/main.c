#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int var = 0;
    int fd = open("./file.txt",O_RDWR|O_CREAT,S_IRUSR|S_IWUSR|S_IROTH|S_IWOTH);
    if(fd == -1)
    {
        perror("can not open file\n");
        exit(-1);
    }
    int pid = -1;
    pid = fork();
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
            write(fd,"I'm child process\n",strlen("I'm child process\n"));
            var = 10;
            printf("var in child = %d\n",var);
            while(1);
            break;
        }
        default:
        {
            printf("I'm parent process with pid = %d, ",getpid());
            printf("My son is %d\n",pid);

            write(fd,"I'm parent process\n",strlen("I'm parent process\n"));

            var = 11;
            printf("var in parent = %d\n",var);
            while(1);
            break;
        }
    }
    return 0;
}