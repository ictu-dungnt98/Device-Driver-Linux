#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

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

char kernel_buf[100];


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
        copy_to_user(buf,kernel_buf,strlen(kernel_buf));
	pr_info("Read device file\n");
	return strlen(kernel_buf);
}
static ssize_t dev_write(struct file *filep, const char __user *buf, size_t len, loff_t *offset)
{
	long int size = 0;
	size = copy_from_user(kernel_buf,buf,len-1);
        printk(KERN_INFO "Write device file value %s\n",kernel_buf);

        return len;
}

static long dev_ioctl(struct file* fd, unsigned int cmd, unsigned long arg)
{
	return 0;
}

static int __init init_example(void)
{

        printk(KERN_INFO "Insmod kernel module driver led\n");

        if(alloc_chrdev_region(&dev,0,1,"my_dev") < 0 )
        {
                printk(KERN_INFO "Error occur, can not register major number\n");
                return -1;
        }
        printk(KERN_INFO "<Major, Minor>: <%d, %d>\n",MAJOR(dev),MINOR(dev));

        if((class_p = class_create(THIS_MODULE,"class_driver_test")) == NULL)
        {
                printk(KERN_INFO "Error occur, can not create class device\n");
                return -1;
        }

        if((device_p = device_create(class_p,NULL,dev,NULL,"driver_test")) == NULL)
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
        cdev_del(&c_dev);
        device_destroy(class_p,dev);
        class_destroy(class_p);
        unregister_chrdev_region(dev,1);
        printk(KERN_INFO "Rmmode kernel module driver led\n");
}

module_init(init_example);
module_exit(exit_example);
