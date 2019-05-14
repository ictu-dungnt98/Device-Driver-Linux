obj-m := gpio.o
gpio-objs := udoo_led.o hw_udooneo_exntended.o

src-file := udoo_led.c

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:

	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
check:
	././checkpatch.pl --no-tree -f $(src-file)

