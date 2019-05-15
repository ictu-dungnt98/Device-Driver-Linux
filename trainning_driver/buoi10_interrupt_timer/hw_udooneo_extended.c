#include "hw_udooneo_extended.h"
#include <linux/string.h>
#include <linux/printk.h>

int gpio_init(unsigned int *gpio,
	unsigned int *iomuxc,
	unsigned char port,
	unsigned char pin, const char *mode)
{
	unsigned long temp = 0;
	unsigned int *gpio_dr = NULL;
	unsigned int *gpio_dir = NULL;
	unsigned int *iomuxc_pin = NULL;

	pr_info("virtual iomuxc = %p\n", iomuxc);
	pr_info("virtual gpio = %p\n", gpio);
	pr_info("port = %d\n", port+1);
        pr_info("pin = %d\n", pin);

	iomuxc_pin = iomuxc + IOMUXC_GPIO1(pin);
	gpio += port;
	gpio_dr = gpio + GPIO_DR;
	gpio_dir = gpio + GPIO_GDIR;

	pr_info("iomuxc   iomuxc_gpio1_%d    iomuxc_pin:\n", pin);
	pr_info("%p +    %d        =      %p\n",(void *)iomuxc, IOMUXC_GPIO1(pin), iomuxc_pin);	
	pr_info("gpio_%d = %p\n", port+1, (void *)gpio);
	pr_info("virtual gpio%d_dr = %p\n", port+1, gpio_dr);
	pr_info("virual gpio%d_dir = %p\n", port+1, gpio_dir);

	/* setting gpio mode */
	temp = read_reg(iomuxc_pin, MUX_MODE_MASK);
	temp |= ALT5;
	write_reg(iomuxc_pin, temp);
	pr_info("set mode succes\n");
	
	/* setting input or output mode */
	if (!strcmp(mode, "INPUT")) {
		temp = read_reg(gpio_dir, ~(0x01 << pin));
		pr_info("value of gpio_dir = %ld\n", temp);
		write_reg(gpio_dir, temp);
		pr_info("setting input mode, value of gpio_dir = %ld\n", read_reg(gpio_dir, 0xFFFFFFFF));
	} else if (!strcmp(mode, "OUTPUT")) {
		temp = read_reg(gpio_dir, ~(0x01 << pin));
		pr_info("value of gpio_dir = %ld\n", temp);
		temp |= 0x01 << pin;
		write_reg(gpio_dir, temp);
		pr_info("setting output mode, value of gpio_dir = %ld\n",read_reg(gpio_dir, 0xFFFFFFFF));

		/* set ouput pin to 1 */
		temp = read_reg(gpio_dr, ~(0x01 << pin));
		pr_info("value of gpio_dr = %d\n", temp);
		temp |= (0x01 << pin);
		write_reg(gpio_dr, temp);
		pr_info("value of gpio_dr = %d\n", temp);
	} else {
		pr_err("Invalid argument mode\n");
		return -1;
	}
	return 0;
}
int gpio_setPin(unsigned int *gpio,
	unsigned char port,
	unsigned char pin,
	unsigned char value)
{
	unsigned int temp = 0;
	unsigned int *dir_reg = NULL;
	unsigned int *port_add = NULL;

	port_add = (gpio + port);
	dir_reg = port_add + GPIO_GDIR;
	/* set output pin bt value */
	temp = read_reg(dir_reg, ~(0x01 << pin));
	write_reg(dir_reg, temp);
	return 0;
}

char gpio_getPin(unsigned int *gpio,
	unsigned char port,
	unsigned char pin)
{
	unsigned int res = 0;
	unsigned int *port_add = NULL;
	unsigned int *gpio_gdir = NULL;

	port_add = gpio + port;
	gpio_gdir = port_add + GPIO_GDIR;
	res = read_reg(gpio_gdir, (0x01 << pin));

	if (res)
		return 1;
	else
		return 0;
}
