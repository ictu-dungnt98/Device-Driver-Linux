#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include "led.h"


#define MAGIC_NUMBER	100
#define BLINK	_IOWR(MAGIC_NUMBER,0,char)
#define ON	_IOWR(MAGIC_NUMBER,1,char)
#define OFF	_IOWR(MAGIC_NUMBER,2,char)

void* gpio_base = NULL;

static int dev_open(struct inode *, struct file *);
static int dev_close(struct inode *, struct file *);
static ssize_t dev_read(struct file *,char __user *, size_t , loff_t *);
static ssize_t dev_write(struct file *, const char __user *, size_t , loff_t *);
static long dev_ioctl(struct file* fd, unsigned int cmd, unsigned long arg);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TRONG DUNG");
MODULE_DESCRIPTION("THIS IS MY FIRST KERNEL MODULE");


static dev_t  dev;
static struct cdev   c_dev;
static struct class  *class_p;
static struct device *device_p;

static struct file_operations fops = {
        .open = dev_open,
        .release = dev_close,
        .read = dev_read,
        .write = dev_write,
	.unlocked_ioctl = dev_ioctl,
};

static int dev_open(struct inode *inodep, struct file *filep)
{
        printk(KERN_INFO "Open device file\n");
        return 0;
}
static int dev_close(struct inode *inodep, struct file *filep)
{
        printk(KERN_INFO "Closed device file\n");
        return 0;
}
static ssize_t dev_read(struct file *filep, char __user *buf, size_t len, loff_t *offset)
{
	unsigned char *kernel_buf = kmalloc(len,GFP_KERNEL);
        printk(KERN_INFO "Read device file %d\n",get_pin(gpio_base,LED_PIN));

	if(get_pin(gpio_base,LED_PIN) != 0)
	{
		*kernel_buf = 0;
		if(copy_to_user(buf,kernel_buf,1))
		{
			printk(KERN_ERR "can not coppy data from kernel space to user space \n");
			return -1;
		}
	}else
	{
		*kernel_buf = 1;
		if(copy_to_user(buf,kernel_buf,1))
		{
			printk(KERN_ERR "can not coppy data from kernel space to user space \n");
			return -1;
		}
	}

	*offset += len;
	kfree(kernel_buf);
	return 0;
}
static ssize_t dev_write(struct file *filep, const char __user *buf, size_t len, loff_t *offset)
{
	char* kernel_buf = kzalloc(len,GFP_KERNEL);
	long int size = 0;
	char cmd = 0;
	if(kernel_buf == NULL)
	{
		return 0;
	}
	size = copy_from_user(kernel_buf,buf,len - 1);
        printk(KERN_INFO "Write device file value %s\n",kernel_buf);

	cmd = kstrtol(kernel_buf,10,&size);
	if(cmd == 0)
	{
		if(size != 0)
		{
			led_on(gpio_base);
		}else
		{
			led_off(gpio_base);
		}
	}
	kfree(kernel_buf);
        return len;
}

static long dev_ioctl(struct file* fd, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		case BLINK:
			printk(KERN_INFO "Blink led\n");
			blink_led(gpio_base);
			break;
		case ON:
			printk(KERN_INFO "Turn on led\n");
			led_on(gpio_base);
			break;
		case OFF:
			printk(KERN_INFO "Turn off led\n");
			led_off(gpio_base);
			break;
		default:
			printk(KERN_INFO "Operator not support\n");
			return -1;

	}
	return 0;
}

static int __init init_example(void)
{

        printk(KERN_INFO "Insmod kernel module driver led\n");
        gpio_base = (unsigned int*)ioremap(GPIO_BASE_ADDRESS,0x100);
        gpio_init(gpio_base);

        if(alloc_chrdev_region(&dev,0,1,"my_dev") < 0 )
        {
                printk(KERN_INFO "Error occur, can not register major number\n");
                return -1;
        }
        printk(KERN_INFO "<Major, Minor>: <%d, %d>\n",MAJOR(dev),MINOR(dev));

        if((class_p = class_create(THIS_MODULE,"class_pi3_led")) == NULL)
        {
                printk(KERN_INFO "Error occur, can not create class device\n");
                return -1;
        }

        if((device_p = device_create(class_p,NULL,dev,NULL,"led_pi3")) == NULL)
        {
                printk(KERN_INFO "Can not create device\n");
                return -1;
        }

        cdev_init(&c_dev,&fops);
        c_dev.owner = THIS_MODULE;
        c_dev.dev = dev;
        cdev_add(&c_dev,dev,1);

        return 0;
}

static void __exit exit_example(void)
{
        led_off(gpio_base);
        cdev_del(&c_dev);
        device_destroy(class_p,dev);
        class_destroy(class_p);
        unregister_chrdev_region(dev,1);
        printk(KERN_INFO "Rmmode kernel module driver led\n");
}

module_init(init_example);
module_exit(exit_example);

