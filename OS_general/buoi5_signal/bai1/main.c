#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void my_handle(int signum)
{
    switch(signum)
    {
        case 2:
        {
            printf("Hello world\n");
            break;
        }
        case 18:
        {
            exit(0);
        }
        default:
        {
            return;
        }
    }
}
int main()
{
    signal(2,my_handle);
    while(1);
    return 0;
}