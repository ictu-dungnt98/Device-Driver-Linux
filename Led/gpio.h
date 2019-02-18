#ifndef _LED_H_
#define _LED_H_

#define GPIO_BASE_ADDRESS               0x3F200000
#define LED_PIN                            4

#define GPIOFSEL0                       0
#define GPIO_SET0                       7
#define GPIO_CLR0                       10
#define GPLEV0				13

#define write_reg(reg,value)    *((unsigned long int*)reg) = value
#define read_reg(reg,mask)      *((unsigned long int*)reg) & (mask)

#define INPUT	0
#define OUTPUT	1



void led_on(unsigned int* gpio_base);
void led_off(unsigned int* addr);
void gpio_init(unsigned int* gpio_base);
char get_pin(unsigned int* gpio_base, unsigned char pin);
char set_pin(unsigned int* gpio_base,unsigned char pin);
void blink_led(unsigned int* gpio_base);
void delay(void);




#endif // _LED_H_




