#include <linux/init.h>
#include <linux/module.h> 
#include <linux/kernel.h>  
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_irq.h>

#define DRIVER_AUTHOR "vu hai long"
#define DRIVER_DSC "led example"

//P8.7 = GPIO2.2
#define  LED		2

//gpio setup  

#define GPIO2_BASE_START	0x481AC000 
#define GPIO2_BASE_STOP		0x481ACFFF 
#define GPIO_IRQSTATUS_SET_0	0x34
#define GPIO_RISINGDETECT	0x148
#define GPIO_IRQSTATUS		0x2C
#define GPIO_DATAIN		0x138
#define GPIO_DATAOUT		0x13C
#define GPIO_SETDATAOUT		0x194
#define GPIO_CLEARDATAOUT	0x190
#define GPIO_OE			0x134

//control mode
#define CONTROL_MODE_START	0x44E10000
#define CONTROL_MODE_STOP	0x44E11FFF

#define P8_7_MODE_OFFSET 	0x890
#define INPUT_PULL_DOWN		0X27 

static int gpio_setup(void);

static void __iomem *io;

static int gpio_setup(void)
{
	unsigned int temp = 0;

	/* set mode ouput & input function for p8.7 */
	io = ioremap(CONTROL_MODE_START, CONTROL_MODE_STOP - CONTROL_MODE_START);

	if (io == NULL) {
		pr_alert("error ioremap control mode\n");
		return -1;
	}
	iowrite32(0x27, (io + P8_7_MODE_OFFSET)); 

	// setup gpio
	io = ioremap(GPIO2_BASE_START, GPIO2_BASE_STOP - GPIO2_BASE_START);

	if (io == NULL) {
		pr_alert("error ioremap gpio setup\n");
		return -1;
	}

	/*enable output at pin LED*/
	temp = ~(1 << LED);
	iowrite32(temp, (io + GPIO_OE));
	return 0;
}
static int __init hello_init(void)
{
	unsigned int temp = 0;

	pr_info("Kernel init\n");
	gpio_setup();

	//turn on led
	iowrite32((1 << LED), (io + GPIO_SETDATAOUT));
	temp = ioread32(io + GPIO_SETDATAOUT);

	return 0;
}

static void __exit hello_exit(void)
{
	iounmap(io);	
	iowrite32((1 << LED), (io + GPIO_CLEARDATAOUT));
	pr_info("Goodbye\n"); 
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DSC);
