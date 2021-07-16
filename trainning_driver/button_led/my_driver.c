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

#define BUTTON1 62
#define BUTTON2 36
#define BUTTON3 32
#define BUTTON4 86 

#define LED1 66 
#define LED2 69 
#define LED3 45 
#define LED4 47 


unsigned int buttons[] = {BUTTON4, BUTTON2, BUTTON3, BUTTON1};
unsigned int leds[] = {LED1, LED2, LED3, LED4};

/* Define interrupt request number  */

static void toggle_leds(int led)
{
	static int pin_value;

	if (led < 0 || led > 3)
		return;

	pin_value = gpio_get_value(leds[led]); 
	
	pin_value = 1 - pin_value;
	
	gpio_set_value(leds[led], pin_value);

        printk(KERN_INFO "Toggle Led%d\n", led);
}

// Create the interrupt handler 
static irqreturn_t my_handler(int irq, void * ident)
{
	int button = -1;

	printk(KERN_INFO "Have interrupt at IRQ%d\n", irq);

	if (irq == gpio_to_irq(BUTTON1))
		button = 0;

	else if (irq == gpio_to_irq(BUTTON2))
		button = 1;

	else if (irq == gpio_to_irq(BUTTON3))
		button = 2;

	else if (irq == gpio_to_irq(BUTTON4))
		button = 3;
	
	if (button < 0 || button > 3)
		return IRQ_HANDLED;

	printk(KERN_INFO "Interrupt received from button%d\n", button);
	toggle_leds(button);

	return IRQ_HANDLED;
}

// Init

static int __init exemple_init (void)
{
	int err;	
	int i;
	int j;

	printk(KERN_INFO "Loading GPIO module \n");


	for(i = 0; i < 4; i++) { 
		/* Leds */
		if ((err = gpio_request(leds[i], THIS_MODULE->name)) !=0) {
			printk(KERN_INFO "Request GPIO output fail\n");
			goto led_init_fail;
			return err;
	        }
		/* Set pin to output mode */
		if ((err = gpio_direction_output(leds[i], 1)) !=0) {
	                goto led_init_fail;
			return err;
	        }

        	/* Buttons */ 
	        if ((err = gpio_request(buttons[i], THIS_MODULE->name)) !=0) {
			goto button_init_fail;
			return err;
	        }
	    	/* Set GPIO_IN to input mode */
	        if ((err = gpio_direction_input(buttons[i])) !=0) {
	                goto button_init_fail;
			return err;
	        }

		gpio_set_debounce(buttons[i], 100);
	
		/* Request BUTTON_IRQ and setting its type to trigger rising */
		printk(KERN_INFO "Requesting irq %d\n", gpio_to_irq(buttons[i]));
		
		err = request_irq(gpio_to_irq(buttons[i]), my_handler,
				IRQF_SHARED | IRQF_TRIGGER_RISING,
				THIS_MODULE->name, THIS_MODULE->name);
		if (err != 0) {
			printk(KERN_INFO "Error %d: could not request irq: %d\n", err, gpio_to_irq(buttons[i]));
			goto request_irq_fail;
			return err;
		}
	
		printk(KERN_INFO "Waiting for interrupts ... \n");

		continue;

request_irq_fail:
		gpio_free(buttons[i]);
button_init_fail:
                gpio_free(leds[i]);
led_init_fail:
		/* free previous gpio pin requested */
                for (j = 0; j < i; j++) {
                        gpio_free(leds[j]);
	                gpio_free(buttons[j]);
			free_irq(gpio_to_irq(buttons[j]), THIS_MODULE->name);
                }

		return err;
	}


	return 0; 

} 

// Clean up : 

static void __exit exemple_exit (void) 
{
	int i = 0;
	
	for(i = 0; i < 4; i++) {
		free_irq(gpio_to_irq(buttons[i]), THIS_MODULE->name);	
		gpio_free(leds[i]);
	}
}

module_init(exemple_init);
module_exit(exemple_exit);

MODULE_AUTHOR("Pierre LE COZ");
MODULE_DESCRIPTION("GPIO interrupt and timers test module for embedded Linux");
MODULE_LICENSE("GPL");


/*
 * End of file
 */
