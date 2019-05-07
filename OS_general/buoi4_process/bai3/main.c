#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int main()
{
    int errnum;
    if (-1 ==setuid(0))
    {
        perror("Error when change UID\n");
        errnum = errno;
        printf("Errno say: %s\n",strerror( errnum));
        exit(-1);
    }
    if(-1 == setgid(0))
    {
        perror("Error when change UID\n");
        exit(-1);
    }

    int fd = open("./file.txt",O_RDWR | O_CREAT, S_IRUSR | S_IWUSR| S_IWOTH| S_IROTH);
    if(fd == -1)
    {
        perror("Can not open file\n");
        exit(-1);
    }
    close(fd);
    return 0;
}