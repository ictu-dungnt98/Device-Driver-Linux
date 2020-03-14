obj-m := led_drv.o
app-file := app

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc $(app-file).c -o $(app-file)
clean
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm $(app-file)
