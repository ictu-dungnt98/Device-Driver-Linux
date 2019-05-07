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

    sigemptyset(&set);
    sigaddset(&set,SIGINT);

	sigprocmask(SIG_BLOCK, &set, &oldset);

	while(1){
		if (!sigpending(&set)){
			if(sigismember(&set, 2)) {
				printf("SIGINT is pending\n");
				exit(0);
			}
		}
	}
}   