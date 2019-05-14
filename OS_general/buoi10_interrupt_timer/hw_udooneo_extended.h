#ifndef _HW_UDOO_EXNTENDED_H_
#define _HW_UDOO_EXTENDED_H_

#define read_reg(reg, mask)	*((unsigned long *)reg) & (mask)
#define write_reg(reg,value)	*((int*)reg) = value

#define GPIO_BASE		0x0209C000u
#define GPIO1			0
#define GPIO2			1000
#define GPIO3			2000
#define GPIO4			3000
#define GPIO5			4000
#define GPIO6			5000
#define GPIO7			6000

#define GPIO_DR			0
#define GPIO_GDIR		1
#define GPIP_PSR		2
#define GPIO_ICR1		3
#define GPIO_ICR2		4
#define GPIO_IMR		5
#define GPIO_ISR		6
#define GPIO_EDGE_SEL		7

#define IOMUXC_BASE		0x020E0000u
#define IOMUXC_GPIO1_0		5	
#define IOMUXC_GPIO1_1		6	
#define IOMUXC_GPIO1_2		7	
#define IOMUXC_GPIO1_3		8	
#define IOMUXC_GPIO1_4		9	
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
	unsigned char pin, const char* mode);
int gpio_setPin(unsigned int *base,
	unsigned char port,
	unsigned char pin,
	unsigned char value);
char gpio_getPin(unsigned int *gpio, unsigned char port, unsigned char pin);



#endif /* _HW_UDOO_EXTENDED_H_*/
