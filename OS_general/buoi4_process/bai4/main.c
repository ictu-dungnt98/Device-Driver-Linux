#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

int main()
{
    int fd = open("./shell.sh",O_RDONLY);
    if(fd == -1)
    {
        perror("File not exist\n");
        exit(-1);
    }

    struct stat mystat;
    fstat(fd,&mystat);


    char c;
    int len_read = 0;
    char cmd[100] = "";
    int i=0;
    while((len_read += (read(fd,&c,1))) < mystat.st_size )
    {
        if(c == '\n' || c == '\r')
        {
            if(cmd[0] == '#')
            {
                memset(cmd,0,sizeof(cmd));
                i = 0;
                continue;
            }
            else
            {
                system(cmd);
                i = 0;
                memset(cmd,0,sizeof(cmd));
            }
        }else
        {

            cmd[i] = c;

            i++;
        }
        c = 0;
    }
    if(cmd[0] != 0)
    {
        system(cmd);
    }
    return 0;
}