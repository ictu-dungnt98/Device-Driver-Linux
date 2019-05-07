#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main()
{
    int fd = -1;
    fd = open("./text.txt",O_RDONLY);
    if(fd < 0)
    {
        perror("file not found\n");
    }
    int n;
    scanf("%d",&n);
    lseek(fd,n,SEEK_SET);
    char buff[1000] = "";
    int read_ = read(fd,buff,sizeof(buff));
    buff[read_] = '\0';
    printf("%s",buff);

    return 0;
}
