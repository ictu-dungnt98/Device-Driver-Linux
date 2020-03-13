app-file := udoo_led.c

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc $(file).c -o $(file)
clean
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm $(file)

check:
	././checkpatch.pl --no-tree -f $(src-file)

