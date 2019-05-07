#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main()
{
    int fd = open("/dev/driver_test",O_RDONLY);
    if(fd < 0)
    {
        perror("can not open device file\n");
        return -1;
    }
    char buff[100] = "";
    int len = read(fd,buff,strlen(buff));
    buff[len] = '\0';
    printf("%s",buff);
    close(fd);
    return 0;
}
