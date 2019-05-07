#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
    if(argc <2)
    {
        printf("program need parameter is path file\n");
        return 1;
    }
    int fd = open(argv[1],O_RDONLY);
    if(fd == -1)
    {
        perror("File not found\n");
        return -1;
    }
    printf("Open file success\n");
    return 0;
}
