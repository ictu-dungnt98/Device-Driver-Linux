#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int i;
    char c;
    int fd;
    char path[20] = "/dev/pts/";
    char terminal[20] = "";

    for(i = 1; i<4;i++)
    {
        memset(terminal,0,sizeof(terminal));
        strcpy(terminal,path);
        terminal[strlen(path)] = i+48;

        fd = open(terminal,O_RDWR);
        if(fd <0)
        {
            printf("Not have terminal link to file %s\n",terminal);
            exit(-1);
        }
        write(fd,"c\n",2);
    }




    return 0;
}