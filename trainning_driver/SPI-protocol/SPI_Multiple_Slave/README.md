******************* GUIDE for use SPI protocol driver as example for studying *************
- This driver is SPI protocol driver and used controll LCD Nokia 5110.
- It can be create multiple device file by just one driver file.
- It can do it because it use Link list to parse device tree SPI node to get child-node and initialize device infomation (SPI slave).
-------------------------------------------------------------------------------------------------------------------------------------
Do following step to use this source code:

1. Modify spi1 node in device tree file : 
	use 0001-spi-patch.patch file with to modify device tree by command:
		patch -p1 < ..(path-directoriy)/0001-spi-patch.patch

2. Re-build kernel and coppy file linux/arch/arm/boot/dts/am335x-boneblack.dtb to BOOT partition of sd-card.
3. Modify path of kernel and toolchain in Makefile file then run following command to build driver:
	make clean all
4. Coppy file lcd_driver.ko to sd-card.

5. ******* Build test tool *******
 - In same folder have two sub-folder is lcd_lib and lcd_lib1 which are two program to above driver.
 - To use them, we just need modify Makefile in each folder with proper path of source linux and toolchain corresponding to out environment build system.
   then call command:
	make clean all
   to build excutable file test. Then coppy these test file to sd-card.

6. Insert sd-card and Power up board and run following command to test driver:
	insmode lcd_driver.ko
	./lcd_test 	- to test spi with slave-0
	./lcd_test1 	- to test spi with slave-1

/******** connection between board and 2 lcd ********/
----------------------------------------------------------------------------------------------------------
|      LCD_1            |                     BBB			|         		LCD_2    |
----------------------------------------------------------------------------------------------------------
       CLK   <-------------------->    P9_31 - SPI1_SCLK           <-------------------->  	CLK
       DIN   <-------------------->    P9_20 - GPIO_112 / SPI1_D1  <-------------------->       DIN
       RESET <-------------------->    P9_27 - GPIO_115            <-------------------->       RESET
       CMD   <-------------------->    P9_25 - GPIO_117            <-------------------->       CMD
       EN    <-------------------->    P9_28 - SPI1_CS0
                                       P9_42 - SPI1_CS1            <-------------------->       EN























