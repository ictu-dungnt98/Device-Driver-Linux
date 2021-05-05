/*
 * Filename: gpio_interrupt_led_module.c    
 *                                                                 	
 * Author:   Pierre LE COZ
 * 		
 * Purposet: Test and demonstrate how to use GPIOs, interrupts and timers in a linux kernel module. 
 *		     
 * Date:     January 2013
 *
 * Informations: This is a linux loadable kernel module.
 * As user space cannot provide access to the GPIO, it is necessary 
 * to develop code for the kernel to manage interrupts and GPIO pin values.
 * The cross-compiler produces a .ko file that can be loaded in the kernel using "insmod" command.
 * "rmmod" removes the module from kernel. 
 * The module is tested on a ARM developement platform embedding a minimal linux system.
 * A RS-232 to USB cable allows to controle the board from the development computer.  
 * The module is build using a cross-toolchaine created with Buildroot. 
 *
 * What the module does: A small electronic circuit is connected to the GPIO.
 * The circuit is made of a push button with its debouncing circuitry and 2 Leds, LED1 and LED2.
 * Once the module is loaded (using "insmod"), the LED1 lights up. 
 * Pressing the push button makes an interrupt request. When the interrupt is received, 
 * the handler call a function that switches leds states: LED1 shut off and LED2 lights up. 
 * A new press will set the system in its original state (LED1 on, LED2 off) etc.
 * A kernel timer schedules the switching function call 1 second after the button press.
 * Finally, Running "cat /proc/interrupts" allows to see interrupts seen from the kernel.      
 *
 */
/* ---- Kernel includes ---- */

#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/timer.h>

// Define inputs and output pins 

#define GPIO_IN 66
#define GPIO_LED1 69
#define GPIO_LED2 45 

// Define the delay to postpone interrupt action 
#define DELAY 1 // 1 second

// Define interrupt request number 
#define BUTTON_IRQ gpio_to_irq(GPIO_IN)

static void switch_leds_state(void)
{
	static int pin_value = 0;

	gpio_set_value(GPIO_LED1, pin_value); 
	
	pin_value = 1 - pin_value;
	
	gpio_set_value(GPIO_LED2, pin_value);

        printk(KERN_INFO "Leds state switched!");
}

// Create the interrupt handler 
static irqreturn_t my_handler(int irq, void * ident)
{
	printk(KERN_INFO "\nInterrupt received from GPIO pin %d", GPIO_IN);
	printk(KERN_INFO "Leds state switch in %d second... ", DELAY);
	
	switch_leds_state();

	return IRQ_HANDLED;
}

// Init

static int __init exemple_init (void)
{
	int err;
	
	printk(KERN_INFO "Loading GPIO module \n");

	// Request GPIO for led 1 :

	if ((err = gpio_request(GPIO_LED1, THIS_MODULE->name)) !=0)
	{
		return err;
        }

	// Set GPIO_LED1 to output mode :

	if ((err = gpio_direction_output(GPIO_LED1, 1)) !=0)
	{
                gpio_free(GPIO_LED1);
                return err;
        }

	// Request GPIO for led 2 : 

	if ((err = gpio_request(GPIO_LED2, THIS_MODULE->name)) !=0)
	{	
                gpio_free(GPIO_LED1);	
		return err;
        }

	// Set GPIO_LED2 to output mode :

	if ((err = gpio_direction_output(GPIO_LED2, 0)) !=0)
	{
                gpio_free(GPIO_LED1);
                gpio_free(GPIO_LED2);
                return err;
        }

        // Request GPIO_IN : 

        if ((err = gpio_request(GPIO_IN, THIS_MODULE->name)) !=0)
	{
     		gpio_free(GPIO_LED1);
                gpio_free(GPIO_LED2);
		return err;
        }
   
    	// Set GPIO_IN to input mode :

        if ((err = gpio_direction_input(GPIO_IN)) !=0)
	{
		gpio_free(GPIO_LED1);
                gpio_free(GPIO_LED2);
                gpio_free(GPIO_IN);
                return err;
        }

	gpio_set_debounce(GPIO_IN, 100);

	// Request BUTTON_IRQ and setting its type to trigger rising : 

	printk(KERN_INFO "Requesting irq %d\n", BUTTON_IRQ);
	if((err = request_irq      (gpio_to_irq(GPIO_IN), my_handler, IRQF_SHARED | IRQF_TRIGGER_RISING, THIS_MODULE->name, THIS_MODULE->name)) != 0)
	{
		printk(KERN_INFO "Error %d: could not request irq: %d\n", err, GPIO_IN);
		gpio_free(GPIO_LED1);	
		gpio_free(GPIO_LED2);
		gpio_free(GPIO_IN);
		return err;
	}
	
	printk(KERN_INFO "Waiting for interrupts ... \n");

	return 0; 

} 

// Clean up : 

static void __exit exemple_exit (void) 
{

	free_irq(gpio_to_irq(GPIO_IN), THIS_MODULE->name);	
	gpio_free(GPIO_LED1);	
	gpio_free(GPIO_LED2);
	gpio_free(GPIO_IN);
}

module_init(exemple_init);
module_exit(exemple_exit);

MODULE_AUTHOR("Pierre LE COZ");
MODULE_DESCRIPTION("GPIO interrupt and timers test module for embedded Linux");
MODULE_LICENSE("GPL");


/*
 * End of file
 */
