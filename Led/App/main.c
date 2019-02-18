#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_SIZE_BUFF	1024
char buff[MAX_SIZE_BUFF] = {0};

#define BLINK	1
#define ON	2
#define OFF	3
#define READ	4
#define WRITE 	5
int main(int argc, char** argv)
{
	int fd = open("/dev/led_pi3",O_WRONLY);
	int size = 0;
	if(fd <0)
	{
		printf("Error occur!\n");
		exit(-1);
	}
	char cmd = 0;
	puts("Choose cmd: 1. BLINK\n2. IOCTL_ON\n3. IOCTL_OFF\n4. READ\n5. WRITE\n6. Quit\n");
	while(1)
	{
		scanf("%d",&cmd);
		getchar();
		switch(cmd)
		{
			case 1:
				puts("Blink");
				ioctl(fd,BLINK);
				break;
			case 2:
				ioctl(fd,ON);
				puts("ON");
				break;
			case 3:
				puts("OFF");
				ioctl(fd,OFF);
				break;
			case 4:
				puts("Read device file:");
				size = read(fd,buff,MAX_SIZE_BUFF);
				puts(buff);
				break;
			case 5:
				puts("Enter data you need write to device.");
				fgets(buff,MAX_SIZE_BUFF,stdin);
				puts(buff);
				size = write(fd,buff,MAX_SIZE_BUFF);
				break;
			case 6:
				puts("exit");
				exit(0);
			default:
				ioctl(fd,cmd);
				break;
		}
	}
	close(fd);
	return 0;
}

