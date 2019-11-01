/* spi_protocol_example.c
 * This is template for SPI_PROTOCOL_DRIVER
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/platform_device.h>


//P8.7 = GPIO2.2
#define  LED		3

//gpio setup  

#define GPIO_IRQSTATUS_SET_0	0x34
#define GPIO_RISINGDETECT		0x148
#define GPIO_IRQSTATUS			0x2C
#define GPIO_DATAIN				0x138
#define GPIO_DATAOUT			0x13C
#define GPIO_SETDATAOUT			0x194
#define GPIO_CLEARDATAOUT		0x190
#define GPIO_OE					0x134

//control mode
#define CONTROL_MODE_START		0x44E10000
#define CONTROL_MODE_STOP		0x44E11FFF

#define P8_7_MODE_OFFSET 		0x890
#define INPUT_PULL_DOWN			0X27 


struct gpio_t {
	void __iomem *base;
	dev_t  dev_num;
	struct cdev   c_dev;
	struct class  *class_p;
	struct device *device_p;
};

struct gpio_t *led;

static int gpio_setup(void)
{
	unsigned int temp = 0;
	static void __iomem *io;

	/* set mode ouput & input function for p8.7 */
	io = ioremap(CONTROL_MODE_START, CONTROL_MODE_STOP - CONTROL_MODE_START);

	if (io == NULL) {
		pr_alert("error ioremap control mode\n");
		return -1;
	}
	iowrite32(0x27, (io + P8_7_MODE_OFFSET)); 

	/*set mode output led*/
	/*save state of this port*/
	iowrite32(~0u, (led->base + GPIO_OE));
	temp = ioread32(led->base + GPIO_OE);

	/*enable output at pin LED*/
	temp &= ~(1 << LED);
	iowrite32(temp, (led->base + GPIO_OE));

	iounmap(io);

	return 0;
}

void led_on(void)
{
	iowrite32((1 << LED), (led->base + GPIO_SETDATAOUT));
}

void led_off(void)
{
	iowrite32((1 << LED), (led->base + GPIO_CLEARDATAOUT));
}

/***************** OS Specific **********************/

static int gpio_open(struct inode *inodep, struct file *filep)
{
	pr_info("Dungnt98 %s, %d\n", __func__, __LINE__);
	return 0;
}

static int gpio_release(struct inode *inodep, struct file *filep)
{
	pr_info("Dungnt98 %s, %d\n", __func__, __LINE__);
	return 0;
}

static int gpio_write(struct file *filep, const char __user *buf,
			size_t len, loff_t *offset)
{
	pr_info("Dungnt98 %s, %d\n", __func__, __LINE__);
	return len;
}

static long gpio_ioctl(struct file *fd, unsigned int cmd,
			unsigned long arg)
{
	return 0;
}

static struct file_operations fops = {
	.open = gpio_open,
	.release = gpio_release,
	.write = gpio_write,
	.unlocked_ioctl = gpio_ioctl,
};

static int my_probe(struct platform_device *pdev)
{
	char ret = 0;
	struct resource *res = NULL;

	led = devm_kzalloc(&pdev->dev, sizeof(*led), GFP_KERNEL);
	if (led == NULL) {
		pr_info("Can not allocate memmory\n");
		return -ENOMEM;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		pr_info("Can not get resource from device tree.\n");
		goto resource_not_exist;
	}

	led->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(led->base)) {
		pr_emerg("Can not mapping memmory\n");
		goto mapping_failed;
	}
/***************** Create Device File ***************/
	ret = alloc_chrdev_region(&led->dev_num, 0, 1, "gpio_led");
	if (ret < 0) {
		pr_info("Error occur, can not register major number\n");
		goto alloc_dev_failed;
	}
	pr_info("<Major, Minor>: <%d, %d>\n",
		MAJOR(led->dev_num), MINOR(led->dev_num));

	led->class_p = class_create(THIS_MODULE, "gpio_class");
	if (led->class_p == NULL) {
		pr_info("Error occur, can not create class device\n");
		goto create_class_failed;
	}

	led->device_p = device_create(led->class_p,
			NULL, led->dev_num, NULL, "gpio_led");
	if (led->device_p == NULL) {
		pr_info("Can not create device\n");
		goto create_device_failed;
	}

	cdev_init(&led->c_dev, &fops);
	led->c_dev.owner = THIS_MODULE;
	led->c_dev.dev = led->dev_num;

	ret = cdev_add(&led->c_dev, led->dev_num, 1);
	if (ret) {
		pr_err("error occur when add properties for struct cdev\n");
		goto cdev_add_fail;
	}

	platform_set_drvdata(pdev, led);

	gpio_setup();
	led_on();

	return 0;

cdev_add_fail:
	device_destroy(led->class_p, led->dev_num);
create_device_failed:
	class_destroy(led->class_p);
create_class_failed:
	unregister_chrdev_region(led->dev_num, 1);
alloc_dev_failed:
	devm_iounmap(&pdev->dev,led->base);
mapping_failed:
resource_not_exist:
	kfree(led);
	return -1;
}

static int my_remove(struct platform_device *pdev)
{
	struct gpio_t *led = platform_get_drvdata(pdev);

	led_off();

	devm_iounmap(&pdev->dev,led->base);
	cdev_del(&led->c_dev);
	device_destroy(led->class_p, led->dev_num);
	class_destroy(led->class_p);
	unregister_chrdev_region(led->dev_num, 1);
	kfree(led);

	pr_emerg("Dungnt98 %s, %d\n", __func__, __LINE__);

	return 0;
}

struct of_device_id dungnt98_of_match[] = {
	{
		.compatible = "dungnt98,gpio2_2"
	},
	{}
};
MODULE_DEVICE_TABLE(of, dungnt98_of_match);

static struct platform_driver my_led_driver = {
	.probe = my_probe,
	.remove = my_remove,
	.driver = {
		.name = "my_led", /* /sys/bus/spi/drivers/.... */
		.owner = THIS_MODULE,
		.of_match_table = dungnt98_of_match,
	},
};

module_platform_driver(my_led_driver);

MODULE_AUTHOR("Trong Dung");
MODULE_LICENSE("GPL");

