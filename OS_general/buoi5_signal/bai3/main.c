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
    signal(2,my_handle);

    sigset_t set, oldset;

    sigemptyset(&set);
    sigaddset(&set,SIGINT);
    sigprocmask(SIG_UNBLOCK, &set, &oldset);
    while(1);


    return 0;
}