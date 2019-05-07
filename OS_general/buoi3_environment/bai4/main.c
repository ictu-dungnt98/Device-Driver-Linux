#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

extern char** environ;
int main()
{
    printf("%s\n",getenv("HOME"));
    printf("%s\n",getenv("USER"));
    printf("%s\n",getenv("PWD"));
    return 0;
}