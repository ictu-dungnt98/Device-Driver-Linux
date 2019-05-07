#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void my_handle(int signum)
{
    switch(signum)
    {
        case SIGINT:
        {
            printf("Hello world\n");
            break;
        }
        default:
        {
            return;
        }
    }
}


int main()
{
    printf("%d\n", SIGRTMAX);
    signal(2,my_handle);

    sigset_t set, oldset;

    sigfillset(&set);
    sigdelset(&set,SIGINT);

    int i;
    for (i = 0; i < NSIG; i++)
    {
        if(sigismember(&set,i))
        {
            printf("%d\n",i);
        }
    }
    

    return 0;
}   