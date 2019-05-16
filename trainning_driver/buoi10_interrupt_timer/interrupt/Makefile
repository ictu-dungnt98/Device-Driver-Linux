obj-m := gpio.o
gpio-objs := udoo_led.o hw_udooneo_extended.o

module-name := gpio.ko

src-file := ./*.c

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo insmod $(module-name)
#	clear
	dmesg
clean:

	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	sudo dmesg -c
	sudo rmmod $(module-name)
	clear
check:
	./checkpatch.pl --no-tree --ignore CONST_STRUCT --show-types -f $(src-file)

