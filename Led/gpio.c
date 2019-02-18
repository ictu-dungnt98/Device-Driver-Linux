#include "led.h"

unsigned int* set_mode;
unsigned int* set_high;
unsigned int* set_low;

unsigned long int j,i;
void set_pin_mode(unsigned int* gpio_base, unsigned char pin,unsigned char mode)
{
	unsigned int temp_reg;

	set_mode = (unsigned int*)(gpio_base + (pin/10));
	temp_reg = read_reg(set_mode, ~(0x07 << ((pin%10)*3)));
	if(mode == INPUT)
	{
		temp_reg |= (0x00u << (pin%10)*3);
		write_reg(set_mode,temp_reg);
	}
	else if (mode == OUTPUT)
	{
		temp_reg |= (0x01u << ((pin%10)*3));
		write_reg(set_mode,temp_reg);
	}
}
char get_pin(unsigned int* gpio_base, unsigned char pin)
{
	unsigned int* gpio_level = (unsigned int*)gpio_base + GPLEV0;
	unsigned int pin_value = read_reg(gpio_level,(0x01 << pin));
	return pin_value;
}
void led_on(unsigned int* gpio_base)
{
        unsigned int temp_reg;
        // set value output is high level
        set_high = gpio_base + GPIO_SET0;
        temp_reg = read_reg(set_high,~(0x01 << 4));
        temp_reg |= 0x01u << 4;
        write_reg(set_high,temp_reg);
}
void led_off(unsigned int* gpio_base)
{
        unsigned int temp_reg;
        // set value on gpio pin is low
        set_low = gpio_base + GPIO_CLR0;
        temp_reg = read_reg(set_low,~(0x01 << 4));
        temp_reg |= 0x01u << 4;
        write_reg(set_low,temp_reg);

}

void delay(void)
{
	int i = 0, j = 0;
	for(i=0;i<1000;i++)
	{
		for(j=0;j<50;j++)
		{
		}
	}
}

void blink_led(unsigned int* gpio_base)
{
	led_off(gpio_base);
	delay();
	led_on(gpio_base);
	delay();
	led_off(gpio_base);
	delay();
	led_on(gpio_base);
	delay();
	led_off(gpio_base);
	delay();
	led_on(gpio_base);
	delay();
}

void gpio_init(unsigned int* gpio_base)
{
        /* set mode gpio pin */
	set_pin_mode(gpio_base, 4,OUTPUT);
        // default led on
        led_on(gpio_base);

}

