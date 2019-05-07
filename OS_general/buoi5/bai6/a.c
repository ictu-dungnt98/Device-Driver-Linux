#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

int counter = 48;

char buffer[1024] = "";
void write_file(int sign_num)
{
    /* open */
    int fd = open("./file.txt", O_RDWR | O_CREAT | O_APPEND, S_IRWXU | S_IRWXO);
    if(fd == -1)
    {
        perror("Can not open file\n");
        exit(-1);
    }
    /* read */
    lseek(fd,SEEK_END,-1);
    counter = 0;
    read(fd,&counter,1);
    counter ++;
    counter += 48;
    /* write */
    int pid_a = getpid();
    memset(buffer,0,1024);
    sprintf(buffer,"\npid of A: %d\n",pid_a);
    write(fd,buffer,strlen(buffer));
    write(fd,&counter,1);
    /* close */
    close(fd);

    /*** SEND SIGNAL ***/
    /* get pid of other process */
    FILE* pipe = popen("pidof b","r");
    char * temp = NULL;
    char pidline[1024] = "";
    int pid_b;
    if(pipe == NULL)
    {
        perror("Error open pile\n");
        exit(-1);
    }
    fgets(pidline,1024,pipe);
    temp = strtok(pidline," ");
    pid_b = atoi(temp);
    pclose(pipe);
    /************************************/
    sleep(3);
    kill(pid_b,31);
    /***-----------END SEND -------------***/

}
int main()
{
    /***    ./a.out to run both a and b program    ***/
    system("gcc -o b b.c");
    signal(30,write_file);
    /* create file */
    int fd = open("./file.txt", O_RDWR | O_CREAT | O_APPEND, S_IRWXU | S_IRWXO);
    if(fd == -1)
    {
        perror("Can not open file\n");
        return -1;
    }

    // get own pid and write file
    int pid_a = getpid();
    memset(buffer,0,1024);
    sprintf(buffer,"pid of A: %d\n",pid_a);
    write(fd,buffer,strlen(buffer));
    write(fd,&counter,1);
    close(fd);
    /*****************************/
    int pid = fork();
    switch(pid)
    {
        case -1:
        {
            perror("Error when call fork() \n");
            exit(-1);
        }
        case 0: // child
        {
            execl("./b","b",NULL);
            break;
        }
        default: //parent
        {
        /*** SEND SIGNAL ***/
            /* get pid of other process */
            FILE* pipe = popen("pidof b","r");
            char * temp = NULL;
            char pidline[1024] = "";
            int pid;
            if(pipe == NULL)
            {
                perror("Error open pile\n");
                exit(-1);
            }
            fgets(pidline,1024,pipe);
            temp = strtok(pidline," ");
            pid = atoi(temp);
            pclose(pipe);
            /************************************/
            sleep(3);
            kill(pid,31);
        /***-----------END SEND -------------***/
            
            while(1);
        }
    }

    return 0;
}