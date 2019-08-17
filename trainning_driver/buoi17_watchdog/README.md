************ Guide for use this source code for example tutorial ***********

1. Modify the compatible property in device tree file linux/arch/arm/boot/dts/am33xx.dtsi
   coressponding with compatible string in file my_watchdog.c.

2. Modify path of source code linux in Makefile file.

3. Modify path of cross-compiler for Beaglebone Black in Makefile file.

4. run command on terminal:
	make clean all
5. Coppy file main and file my_watchdog.ko to sd-card then insert it to Beaglebone Black.

6. Power on board, then run following command to insert driver:
	insmod my_watchdog.ko

7. run following command to run test program for watchdog driver:
	./main

