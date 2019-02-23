#ifndef _LCD_
#define _LCD_

#include <linux/delay.h>


/*Define for GPIO*/
#define GPIO_BASE_ADDR		0x3F200000
#define GPSET0			7
#define GPCLR0			10
#define GPLVL0			13

/*Define for LCD*/
#define FUNCTION		0x38
#define CONTROL			0x0c
#define CLRSCR			0x01
#define RETHOME			0x03
#define ENTRYMODE		0x06

typedef enum direction {
	INPUT = 0,
	OUTPUT = 1
} Direction;

void gpio_init(unsigned int *base_addr);
void gpio_set_output(unsigned int *base_addr);
int get_pin_state(unsigned int *base_addr, int pin);
void gpio_set_direction(unsigned int *base_addr, Direction direct, int pin);
void gpio_set_value(unsigned int *base_addr, int pin, int value);
void set_data_to_low(unsigned int *base_addr);
void lcd_send(unsigned int *base_addr, unsigned char cmd);
void lcd_reset(unsigned int *base_addr, int value);
void lcd_rw(unsigned int *base_addr, int value);
void lcd_enable(unsigned int *base_addr, int value);
void wait_busy(unsigned int *base_addr);
void write_cmd(unsigned int *base_addr, unsigned char byte);
void write_char(unsigned int *base_addr, unsigned char c);
void write_string(unsigned int *base_addr, char *str);
void goto_xy(unsigned int *base_addr, unsigned char row, unsigned char col);
void clear_screen(unsigned int *base_addr);
void setup_lcd(unsigned int *base_addr);

#endif

