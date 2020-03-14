/*
 * GPIO support.
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 or
 * later as publishhed by the Free Software Foundation.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define DRIVER_AUTHOR "nguyentrongdung0498@gmail.com"
#define DRIVER_DSC "led example"

/* gpio setup */
#define GPIO2_BASE_START    0x481AC000 
#define GPIO2_BASE_STOP    0x481ACFFF 
#define GPIO_IRQSTATUS_SET_0    0x34
#define GPIO_RISINGDETECT    0x148
#define GPIO_IRQSTATUS    0x2C
#define GPIO_DATAIN    0x138
#define GPIO_DATAOUT    0x13C
#define GPIO_SETDATAOUT    0x194
#define GPIO_CLEARDATAOUT    0x190
#define GPIO_OE    0x134

/* control mode */
#define CONTROL_MODE_START    0x44E10000
#define CONTROL_MODE_STOP    0x44E11FFF

#define P8_7_MODE_OFFSET    0x890
#define INPUT_PULL_DOWN    0X27 

/* P8.7 = GPIO2.2 */
#define  LED        2

static dev_t  dev;
static struct cdev   c_dev;
static struct class  *class_p;
static struct device *device_p;

static void __iomem *io;

/*
* dev_open(struct inode *inodep, struct file *filep)
* Called when open device file.
* This functino setting P8.7 pin to gpio output mode
* return: 0 if success, other for false.
*/
static int dev_open(struct inode *inodep, struct file *filep)
{
    /* set mode ouput for p8.7 */
    io = ioremap(CONTROL_MODE_START, CONTROL_MODE_STOP - CONTROL_MODE_START);
    if (io == NULL) {
        pr_alert("error ioremap control mode\n");
        return -EFAULT;
    }
    iowrite32(0x27, (io + P8_7_MODE_OFFSET)); 

    /*enable output at pin LED*/
    io = ioremap(GPIO2_BASE_START, GPIO2_BASE_STOP - GPIO2_BASE_START);
    if (io == NULL) {
        pr_alert("error ioremap gpio setup\n");
        return -EFAULT;
    }
    iowrite32(~(1 << LED), (io + GPIO_OE));

    iounmap(io);

    pr_info("Open device file\n");
    return 0;
}
/* 
* dev_release(struct inode *inodep, struct file *filep)
* This function called when application from userspace close device file after
* opening.
* Return: 0 for success.
*/
static int dev_release(struct inode *inodep, struct file *filep)
{
    pr_info("Closed device file\n");
    return 0;
}

/*
* dev_read(struct file *filep, char __user *buf, size_t len, loff_t *offset)
* This function called when userspace read device file
* Return: 0 for success, other for false.
*/
static ssize_t dev_read(struct file *filep, char __user *buf,
    size_t len, loff_t *offset)
{
    unsigned int temp;

    io = ioremap(GPIO2_BASE_START, GPIO2_BASE_STOP - GPIO2_BASE_START);
    if (io == NULL)
        goto ioremap_fail;

    temp = ioread32(io + GPIO_SETDATAOUT);
    if (temp == 0) {
        temp = copy_to_user(buf, "1", 1);
		if(temp != 0) {
            pr_err("can't coppy to user \n");    
            goto send_data_fail;
        }
    } else {
        temp = copy_to_user(buf, "0", 1);
        if(temp != 0) {
            pr_err("can't coppy to user \n");    
            goto send_data_fail;
        }
    }

    iounmap(io);

    pr_info("Read device file\n");
    return 0;

send_data_fail:
    iounmap(io);
ioremap_fail:
    return -EFAULT;
}

/* 
* dev_write(struct file *filep, const char __user *buf, size_t len,
*           loff_t *offset)
*  dev_write function called when userspace write to device file of this driver.
* Return: 0 for success and stop writing.
*/
static ssize_t dev_write(struct file *filep, const char __user *buf,
    size_t len, loff_t *offset)
{
    long int temp;
    char *kernel_buf = NULL;

    kernel_buf = kzalloc(len, GFP_KERNEL);
    if (kernel_buf == NULL)
	    goto malloc_fail;

    temp = copy_from_user(kernel_buf, buf, len);
    if(temp != 0)
        goto get_data_fail;

    kernel_buf[len] = '\0';

    io = ioremap(GPIO2_BASE_START, GPIO2_BASE_STOP - GPIO2_BASE_START);
    if (io == NULL)
	    goto get_data_fail;

    if (!strcmp(kernel_buf,"0")) {
        pr_info("Write value 0\n");
        iowrite32((1 << LED), (io + GPIO_CLEARDATAOUT)); /* Led low */
    } else {
        iowrite32((1 << LED), (io + GPIO_SETDATAOUT));
        pr_info("Write value 1\n");
    }

    iounmap(io);
    kfree(kernel_buf);
    kernel_buf = NULL;

    pr_info("kernel_buf: s= %s d=%ld\n", kernel_buf, temp);
    return len;

get_data_fail:
    kfree(kernel_buf);
    kernel_buf = NULL;
malloc_fail:
    return -EFAULT;
}

static const struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

static int __init init_example(void)
{
    pr_info("Insmod kernel module driver led\n");

    if (alloc_chrdev_region(&dev, 0, 1, "my_dev") < 0) {
        pr_info("Error occur, can not register major number\n");
        return -EFAULT;
    }

    class_p = class_create(THIS_MODULE, "class_driver_test");
    if (class_p == NULL) {
        pr_info("Error occur, can not create class device\n");
        return -1;
    }

    device_p = device_create(class_p, NULL, dev, NULL, "led_drv");
    if (device_p == NULL) {
        pr_info("Can not create device\n");
        return -EFAULT;
    }

    cdev_init(&c_dev, &fops);
    c_dev.owner = THIS_MODULE;
    c_dev.dev = dev;
    cdev_add(&c_dev, dev, 1);

    return 0;
}

static void __exit exit_example(void)
{
    cdev_del(&c_dev);
    device_destroy(class_p, dev);
    class_destroy(class_p);
    unregister_chrdev_region(dev, 1);
    pr_info("Rmmode kernel module driver led\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TRONG DUNG");
MODULE_DESCRIPTION("THIS IS MY FIRST KERNEL MODULE");
module_init(init_example);
module_exit(exit_example);
