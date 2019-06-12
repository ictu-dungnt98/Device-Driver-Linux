obj-m += omap_wdt.o

PWD := $(shell pwd)

GCC := /usr/bin/arm-linux-gnueabihf-
KERNEL := /home/trongdung/linux/

all:
	make  ARCH=arm CROSS_COMPILE=$(GCC) -C $(KERNEL) SUBDIRS=$(PWD) modules
clean:
	make  -C $(KERNEL) SUBDIRS=$(PWD) clean
