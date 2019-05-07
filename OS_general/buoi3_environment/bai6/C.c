#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    pid_t pid = getpid();

    printf("process C session id = %d\n",getsid(pid));

    return 0;
}