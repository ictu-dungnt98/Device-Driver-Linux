#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
/*
* Author: nguyentrongdung0498@gmail.com
* 0982634613
*/
int main()
{
	char data[10] = "";
	int fd;
	char choice = 0;

	fd = open("/dev/led_drv", O_RDWR);
	if (fd < 0)
	    return -1;

	while(1) {
		puts("");
		puts("Choose one:");
		puts("0: Read status Led's status");
		puts("1: Turn on led");
		puts("2: Turn off led");
		puts("3: Exit");
		scanf("%d",&choice);
		getchar();

		switch(choice) {
			case 0: {
				memset(data, 0, sizeof(data));
				read(fd, data, sizeof(data));
				puts(data);
				break;
			}
			case 1: {
				puts("Turn on led");
				write(fd, "1", 1);
				break;
			}
			case 2: {
				puts("Turn off led");
				write(fd, "0", 1);
				break;
			}
			case 3: {
				puts("Exit");
				return 0;
			}
			default: {
				break;
			}
		}

	}
}
