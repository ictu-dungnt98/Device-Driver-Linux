#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    char c;
    scanf("%c",&c);
    printf("read in C %c",c);
    return 0;
}