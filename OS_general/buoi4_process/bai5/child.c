#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

int main()
{
    if (-1 == system("ls > file.txt"))
    {
        perror("can not run command\n");
        return -1;
    }
    return 0;
}