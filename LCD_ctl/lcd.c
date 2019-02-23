#include "lcd.h"

/*set gpio from gpio9 to gpio19 output
*GPIO from 9 to 16 spend for data transfer
*/



int gpio_pin[11] = {9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

/*void gpio_set_output(unsigned int *base_addr)
{
	int i;

	for (i = 0; i < 11; i++) {
		*(base_addr + (gpio_pin[i] / 10)) = (*(base_addr + (gpio_pin[i] / 10)) &
				 ~(7 << 21) | (1 << ((gpio_pin[i] % 10) * 3) ));
	}
}*/

void gpio_set_direction(unsigned int *base_addr, Direction direct, int pin)
{
	if (direct == INPUT)
		*(base_addr + (pin / 10)) = (*(base_addr + (pin /10)) & ~(7 << ((pin % 10)*3)) |
						(0 << ((pin % 10) * 3)));
	else
		*(base_addr + (pin / 10)) = (*(base_addr + (pin /10)) & ~(7 << ((pin % 10)*3)) |
						(1 << ((pin % 10) * 3)));
}

void gpio_set_output(unsigned int *base_addr)
{
	int i;

	for(i = 0; i < 11; i++) {
		gpio_set_direction(base_addr, OUTPUT, gpio_pin[i]);
	}
}

int get_pin_state(unsigned int *base_addr, int pin)
{
	gpio_set_direction(base_addr, INPUT, pin);
	return (*(base_addr + GPLVL0) & (1 << pin));
}

void gpio_init(unsigned int *base_addr)
{
	gpio_set_output(base_addr);
}

void gpio_set_value(unsigned int *base_addr, int pin, int value)
{
	if (value)
		*(base_addr + GPSET0) = 1 << pin;
	else
		*(base_addr + GPCLR0) = 1 << pin;
}

void set_data_to_low(unsigned int *base_addr)
{
	int i;

	for (i = 0; i < 8; i++) {
		gpio_set_value(base_addr, gpio_pin[i], 0);
	}
}

void lcd_send(unsigned int *base_addr, unsigned char cmd)
{
	int i;
	int tmp;

	//set_data_to_low(base_addr);
	for (i = 7; i >= 0; i--) {
		tmp = (cmd & (1 << (7 - i))) > 0;
		gpio_set_value(base_addr, gpio_pin[i], tmp);
	}
}

/*GPIO17 for reset*/
void lcd_reset(unsigned int *base_addr, int value)
{
	gpio_set_value(base_addr, 17, value);
}

/*GPIO18 for read write mode*/
void lcd_rw(unsigned int *base_addr, int value)
{
	gpio_set_value(base_addr, 18, value);
}

/*GPIO19 for enable lcd*/
void lcd_enable(unsigned int *base_addr, int value)
{
	gpio_set_value(base_addr, 19, value);
}

void wait_busy(unsigned int *base_addr)
{
	int busy;

	gpio_set_value(base_addr, 9, 1);
	lcd_reset(base_addr, 0);
	lcd_rw(base_addr, 1);

	/*conect gpio 9 to D7 of lcd*/
	gpio_set_direction(base_addr, INPUT, 9); /*set pin 9 to input*/
	while (1) {
		lcd_enable(base_addr, 1);
		msleep(1);
		busy = get_pin_state(base_addr, 9);
		lcd_enable(base_addr, 0);
		msleep(1);
		if (!busy)
			break;
	}

	/*set pin 9 to output*/
	gpio_set_direction(base_addr, OUTPUT, 9);
}

void write_cmd(unsigned int *base_addr, unsigned char byte)
{
	wait_busy(base_addr);
	lcd_reset(base_addr, 0);
	lcd_rw(base_addr, 0);
	msleep(1);
	lcd_send(base_addr, byte);
	lcd_enable(base_addr, 1);
	msleep(1);
	lcd_enable(base_addr, 0);
}

void write_char(unsigned int *base_addr, unsigned char c)
{
	wait_busy(base_addr);
	lcd_reset(base_addr, 1);
	lcd_rw(base_addr, 0);
	msleep(1);
	lcd_send(base_addr, c);
	lcd_enable(base_addr, 1);
	msleep(1);
	lcd_enable(base_addr, 0);
}

void write_string(unsigned int *base_addr, char *str)
{
	int i = 0;

	while (str[i]) {
		write_char(base_addr, str[i]);
		i++;
	}
}

void goto_xy(unsigned int *base_addr, unsigned char row, unsigned char col)
{
	unsigned char ac = 0x80;

	ac = ac | ((row & 1) << 6);
	ac = ac | (col & 15);
	write_cmd(base_addr, ac);
}

void clear_screen(unsigned int *base_addr)
{
	write_cmd(base_addr, CLRSCR);
	msleep(1);
}

void setup_lcd(unsigned int *base_addr)
{
	gpio_init(base_addr);
	msleep(2);
	write_cmd(base_addr, 0x30);
	msleep(2);
	write_cmd(base_addr, 0x30);
        msleep(2);
	write_cmd(base_addr, 0x30);
        msleep(2);
	lcd_enable(base_addr, 1);
	msleep(2);
	lcd_enable(base_addr, 0);
	wait_busy(base_addr);
	write_cmd(base_addr, FUNCTION);
	write_cmd(base_addr, CONTROL);
	write_cmd(base_addr, CLRSCR);
	write_cmd(base_addr, ENTRYMODE);
}

