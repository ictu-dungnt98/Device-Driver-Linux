#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <errno.h>


int counter = 48;

char buffer[1024] = "";
void write_file(int sign_num)
{
    /* open */
    int fd = open("./file.txt", O_RDWR | O_CREAT | O_APPEND, S_IRWXU | S_IRWXO);
    if(fd == -1)
    {
        perror("Can not open file\n");
        printf("In A, open failed, errno:%s\n",strerror(errno));
        exit(-1);
    }
    /* read */
    if(lseek(fd,-1,SEEK_END) < 0)
    {
        perror("Process A. Can not seek file\n");
        printf("In A, lseek failed, errno:%s\n",strerror(errno));
        exit(-1);
    }
    counter = 0;
    if(read(fd,&counter,1) <0 )
    {
        perror("Can not read from A process\n");
        printf("In A,read failed, errno:%s\n",strerror(errno));
        exit(-1);
    }
    counter ++;
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
        printf("In A, popen failed, errno:%s\n",strerror(errno));
        exit(-1);
    }
    fgets(pidline,1024,pipe);
    temp = strtok(pidline," ");
    pid_b = atoi(temp);
    pclose(pipe);
    /************************************/
    sleep(3);
    kill(pid_b,SIGUSR2);
    /***-----------END SEND -------------***/

}
int main()
{
    /***    ./a.out to run both a and b program    ***/
    system("gcc -o b b.c");
    signal(SIGUSR1,write_file);

    /* create file */
    int fd = open("./file.txt", O_RDWR | O_CREAT | O_APPEND, S_IRWXU | S_IRWXO);
    if(fd == -1)
    {
        perror("Can not open file\n");
        printf("In A, main open failed, errno:%s\n",strerror(errno));
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
            kill(pid,SIGUSR2);
        /***-----------END SEND -------------***/
            
            while(1);
        }
    }

    return 0;
}