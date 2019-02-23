#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/delay.h>

#include "lcd.h"

static int count;
static char message[50];
static char buff[20] = "nguyen thanh tung\n";
unsigned int *gpio_addr;

static int dev_open(struct inode *, struct file *);
static int dev_close(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_close,
};

static struct miscdevice my_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "lcd_ctl",
	.fops = &fops,
};

static int dev_open(struct inode *inodep, struct file *filep)
{
	pr_info("open file\n");
	return 0;
}

static int dev_close(struct inode *inodep, struct file *filep)
{
	count = 0;
	pr_info("file is closed\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buf, size_t len,
			loff_t *offset)
{
	count++;
	pr_info("read file\n");
	if (count > 1)
		return 0;
	return sprintf(buf, "%s", buff);
}

static int get_pos_char(char *str, char c)
{
	int i;

	for (i = 0; i < strlen(str); i++) {
		if (str[i] == c)
			return i;
	}
	return 0;
}

static ssize_t dev_write(struct file *filep, const char *buf, size_t len,
			loff_t *offset)
{
	int row = 0;
	int pos;
	char f_str[20];

	clear_screen(gpio_addr);
	memset(message, 0, strlen(message));
	memset(f_str, 0, 20);
	copy_from_user(message, buf, len - 1);
	pr_info("get from user: %s\n", message);

	pos = get_pos_char(message, '-');
	if (pos) {
		goto_xy(gpio_addr, 0, 0);
		strncpy(f_str, message, pos);
		write_string(gpio_addr, f_str);
		goto_xy(gpio_addr, 1, 0);
		write_string(gpio_addr, message + pos +1);
	} else {
		sscanf(message, "%d", &row);
		if (row == 0) {
			write_string(gpio_addr, message);
		} else {
			goto_xy(gpio_addr, 1, 0);
			write_string(gpio_addr, message + 2);
		}
	}
	return len;
}

static int __init example_init(void)
{
	int ret;

	gpio_addr = (unsigned int *)ioremap(GPIO_BASE_ADDR, 0x100);
	setup_lcd(gpio_addr);
	write_string(gpio_addr, "hello");
	ret = misc_register(&my_dev);
	if (ret) {
		pr_alert("can not register misc device\n");
		return ret;
	}

	pr_info("register device successfully with minor number is: %d\n",
			my_dev.minor);
	return ret;
}

static void __exit example_exit(void)
{
	misc_deregister(&my_dev);
	clear_screen(gpio_addr);
	set_data_to_low(gpio_addr);
	iounmap(gpio_addr);
	pr_info("goodbye\n");
}

module_init(example_init);
module_exit(example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("tungnt58-khotv");
MODULE_VERSION("0.1");

