#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    pid_t pid = getpid();
    sleep(3);
    printf("process group id C = %d\n",getpgid(pid));
    return 0;
}