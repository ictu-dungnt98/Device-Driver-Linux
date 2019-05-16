#include "hw_udooneo_extended.h"
#include <linux/string.h>
#include <linux/printk.h>

int gpio_init(unsigned int *gpio,
	unsigned int *iomuxc,
	unsigned char port,
	unsigned char pin, const char mode)
{
	unsigned long temp = 0;
	unsigned int *gpio_dr = NULL;
	unsigned int *gpio_dir = NULL;
	unsigned int *gpio_edge = NULL;
	unsigned int *iomuxc_pin = NULL;

	gpio += port;
	gpio_dr = gpio + GPIO_DR;
	gpio_dir = gpio + GPIO_GDIR;
	gpio_edge = gpio + GPIO_EDGE_SEL;
	iomuxc_pin = iomuxc + IOMUXC_GPIO1(pin);

	/* setting gpio mode */
	temp = read_reg(iomuxc_pin, MUX_MODE_MASK);
	temp |= ALT5;
	write_reg(iomuxc_pin, temp);
	pr_info("set mode succes\n");

	/* setting input or output mode */
	if (mode & INPUT) {
		temp = read_reg(gpio_dir, ~(0x01 << pin));
		write_reg(gpio_dir, temp);
		pr_info("setting input mode\n");

		if (mode & INTERRUPT){
			temp = read_reg(gpio_edge, ~(0x01 << pin));
			temp |= (0x01 << pin);
			write_reg(gpio_edge,temp);
			pr_info("setting interrupt mode for GPIO_%d%d\n",port+1,pin);
		}
	} else if (mode & OUTPUT) {
		temp = read_reg(gpio_dir, ~(0x01 << pin));
		temp |= 0x01 << pin;
		write_reg(gpio_dir, temp);
		pr_info("setting output mode\n");

		/* set ouput pin to 1 */
		gpio_setPin(gpio, port, pin, 1);
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
	unsigned int *gpio_dr = NULL;

	gpio  = gpio + port;
	gpio_dr = gpio + GPIO_DR;

	/* write value to pin */
	temp = read_reg(gpio_dr, ~(0x01 << pin));
	temp |= (value << pin);
	write_reg(gpio_dr, temp);
	
	pr_info("value of gpio_dr after set = %d\n", temp);

	return 0;
}

unsigned char gpio_getPin(unsigned int *gpio,
	unsigned char port,
	unsigned char pin)
{
	unsigned char res = 0;
	unsigned int *gpio_dr = NULL;

	gpio += port;
	gpio_dr = gpio + GPIO_DR;
	res = read_reg(gpio_dr, (0x01 << pin));

	if (res)
		return 1;
	else
		return 0;
}
