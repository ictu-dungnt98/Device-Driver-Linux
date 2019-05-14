#include "hw_udooneo_extended.h"
#include <linux/string.h>
#include <linux/printk.h>

int gpio_init(unsigned int *gpio,
	unsigned int *iomuxc,
	unsigned char port,
	unsigned char pin, const char *mode)
{
	unsigned int temp = 0;
	unsigned int *dir_reg = NULL;
	unsigned int *port_add = NULL;

	iomuxc += (port + pin);
	port_add = gpio + port;
	dir_reg = port_add + GPIO_GDIR;
	gpio += (port + pin);

	/* setting gpio mode */
	temp = read_reg(iomuxc, MUX_MODE_MASK);
	temp |= ALT5;
	write_reg(iomuxc, temp);
	/* setting input or output mode */
	if (!strcmp(mode, "INPUT")) {
		temp = read_reg(gpio, ~(0x01 << pin));
		write_reg(gpio, temp);
	} else if (!strcmp(mode, "OUTPUT")) {
		temp = read_reg(gpio, ~(0x01 << pin));
		temp |= 0x01 << pin;
		write_reg(gpio, temp);
		/* set ouput pin to 1 */
		temp = read_reg(dir_reg, ~(0x01 << pin));
		temp |= (0x01 << pin);
		write_reg(dir_reg, temp);
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
