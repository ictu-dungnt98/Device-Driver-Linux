obj-m := gpio.o
gpio-objs := udoo_led.o hw_udooneo_exntended.o

src-file := ./*.c

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:

	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
check:
	./checkpatch.pl --no-tree --ignore CONST_STRUCT --show-types -f $(src-file)

