// Compile : gcc -Wall uart-receive.c -o uart-receive -lwiringPi

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int fd;

void clean_stdin(void)
{
    int c;

    printf("clean stdin ");

    do {
        c = getchar();
    } while (c != '\n' && c != EOF);

    printf("done\n");
}

static void *thread1_handle(void *arg)
{
    int cmd;
    char command[256];

    while (1) {
        memset(command, 0, sizeof(command));
        cmd = 0;

        printf("Select command to send\n");
        scanf("%d", &cmd);

        clean_stdin();

        switch (cmd) {
            case 1: {
                sprintf(command,
                        "{\"cmd_type\":0, \"brand\":12, \"power\":0, \"temp\":18, \"mode\":1, \"fan\":1, \"swing\":1, "
                        "\"model\": 4}");
            } break;

            case 2: {
                sprintf(command,
                        "{\"cmd_type\":1, \"brand\":12, \"power\":0, \"temp\":18, \"mode\":1, \"fan\":1, \"swing\":1, "
                        "\"model\": 4}");
            } break;

            case 3: {
                sprintf(command,
                        "{\"cmd_type\":2, \"brand\":0, \"power\":1, \"temp\":18, \"mode\":1, \"fan\":1, \"swing\":1, "
                        "\"model\": 4}");
            } break;

            case 4: {
                sprintf(command,
                        "{\"cmd_type\":2, \"brand\":0, \"power\":0, \"temp\":18, \"mode\":1, \"fan\":1, \"swing\":1, "
                        "\"model\": 4}");
            } break;

            default:
                break;
        }

        write(fd, command, strlen(command));
        fsync(fd);
    }

    return 0;
}

static void *thread2_handle(void *arg)
{
    char buff[512];

    int rx_data_len;

    printf("thread2 run\n");

    while (1) {
        memset(buff, 0, sizeof(buff));
        rx_data_len = 0;

        rx_data_len = read(fd, buff, 512);
        
        if (rx_data_len > 0)
            printf("%s", buff);

        usleep(10000);
    }

    printf("thread2 terminate\n");

    return 0;
}

int main(int argc, char** argv)
{
    pthread_t thread1, thread2;

    if (argc < 2) {
	    printf("try again with syntax as: ./%s /dev/ttyx with x is 0 1 2 ...\n", argv[0]);
	    return 0;
    }


    fd = open(argv[1], O_RDWR | O_NOCTTY | O_TRUNC | O_SYNC);
    if (fd) {
        printf("Can not open serial port. %s\n", strerror(errno));
    }

    if (0 != pthread_create(&thread1, NULL, thread1_handle, NULL)) {
        perror("Can not create thread 1\n");
        exit(-1);
    }

    if (0 != pthread_create(&thread2, NULL, thread2_handle, NULL)) {
        perror("Can not create thread 2\n");
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}
