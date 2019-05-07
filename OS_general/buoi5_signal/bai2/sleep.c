#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void handle(int signum)
{
    switch(signum)
    {
        case 12:
        {

        }
    }
}
int main()
{
    signal(12,handle);
    printf("pid of child is %d\n",getpid());

    sleep(1000);
    printf("I'm get up\n");
    
    return 1;
}