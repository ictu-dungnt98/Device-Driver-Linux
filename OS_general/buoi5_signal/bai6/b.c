#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

int counter = 0;
char buffer[1024] = "";
void write_file(int sign_num)
{
    /* open file */
    int fd = open("./file.txt", O_RDWR | O_APPEND, S_IRWXU | S_IRWXO);
    if(fd == -1)
    {
        perror("Can not open file\n");
        printf("In B, open failed, errno:%s\n",strerror(errno));
        exit(-1);
    }
    /* read */
    if(lseek(fd, -1, SEEK_END) < 0)
    {
        perror("Process B. Can not seek file\n");
        printf("In B, lseek failed, errno:%s\n",strerror(errno));
        exit(-1);
    }
    if(read(fd,&counter,1) < 0 )
    {
        perror("Can not read from A process\n");
        printf("In B, read failed, errno:%s\n",strerror(errno));
        exit(-1);
    }                             
    printf("\ncoutner = %d\n",counter);
    counter ++;
    /* write */
    int pid_b = getpid();
    memset(buffer,0,1024);
    sprintf(buffer,"\npid of B: %d\n",pid_b);

    write(fd,buffer,strlen(buffer));
    write(fd,&counter,1);
    /* close */
    close(fd);
    /*****************************************************/
/******** SEND SIGNAL *********/    
    /* get pid of other process */
    FILE* pipe = popen("pidof a.out","r");
    char* temp = NULL;
    char pidline[1024] = "";
    int pid_a;

    if(pipe == NULL)
    {
        perror("Error popen pile\n");
        printf("In B, popen failed, errno:%s\n",strerror(errno));
        exit(-1);
    }
    fgets(pidline,1024,pipe);
    temp = strtok(pidline," ");
    pid_a = atoi(temp);
    pclose(pipe);
    /************************************/
    sleep(3);
    kill(pid_a,SIGUSR1);
/********** END SEND ***********/
}
int main()
{
    
    signal(SIGUSR2,write_file);
    while(1);
    return 0;
}