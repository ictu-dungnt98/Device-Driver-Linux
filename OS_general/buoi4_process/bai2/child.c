#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        perror("Can not run program because missing parameter\n");
        exit(-2);
    }
    int fd = open(argv[1],O_RDWR | O_CREAT,O_RDWR|O_CREAT|S_IRUSR|S_IWUSR|S_IWOTH|S_IROTH);
    if(fd == -1)
    {
        perror("can not open file\n");
        exit(-1);
    }

    write(fd,"Hello I'm Child\n",strlen("Hello I'm Child\n"));
    close(fd);
    return 0;
}