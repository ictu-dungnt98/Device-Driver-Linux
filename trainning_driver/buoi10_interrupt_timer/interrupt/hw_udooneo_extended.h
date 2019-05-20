#ifndef _HW_UDOO_EXNTENDED_H_
#define _HW_UDOO_EXTENDED_H_

#define read_reg(reg, mask)	*((unsigned long *)reg) & (mask)
#define write_reg(reg,value)	*((unsigned long*)reg) = value

#define GPIO_BASE		0x0209C000u
#define GPIO(x)			(((int)x - 1) * 1000)			

#define	INPUT			(0x01 << 0)
#define OUTPUT			(0x01 << 1)
#define INTERRUPT		(0x01 << 2)

#define GPIO_DR			0
#define GPIO_GDIR		1
#define GPIP_PSR		2
#define GPIO_ICR1		3
#define GPIO_ICR2		4
#define GPIO_IMR		5
#define GPIO_ISR		6
#define GPIO_EDGE_SEL		7

#define INT_PIN(x)		(((int)x) * 2)
#define INT_MASK		0x03
#define IRQ_LOW_LEVEL		0x00
#define IRQ_HIGH_LEVEL		0x01
#define IRQ_RAISING_EDGE	0x10
#define IRQ_FALLING_EDGE	0x11

#define IOMUXC_BASE		0x020E0000u
#define IOMUXC_GPIO1(x)		(5+((int)x))	

#define MUX_MODE_MASK		(~(0x0F << 0))

#define ALT0			(0x00 << 0)
#define ALT1			(0x01 << 0)
#define ALT2			(0x02 << 0)
#define ALT3			(0x03 << 0)
#define ALT4			(0x04 << 0)
#define ALT5			(0x05 << 0) 


int gpio_init(unsigned int *gpio,
	unsigned int *iomuxc,
	unsigned char port,
	unsigned char pin, const char mode);
int gpio_setPin(unsigned int *base,
	unsigned char port,
	unsigned char pin,
	unsigned char value);
unsigned char gpio_getPin(unsigned int *gpio, unsigned char port, unsigned char pin);



#endif /* _HW_UDOO_EXTENDED_H_*/
