#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/of.h>
#include <linux/serial_reg.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/wait.h>

#define SERIAL_RESET_COUNTER (0)
#define SERIAL_GET_COUNTER (1)
#define SERIAL_BUFSIZE 16

struct feserial_dev {
	struct miscdevice miscdev;
	void __iomem *regs;
	u32 write_count;
	int irq;
	char serial_buf[SERIAL_BUFSIZE];
	int serial_buf_rd;
	int serial_buf_wr;
	struct dentry *debugfs_parent;
	wait_queue_head_t wq;
	spinlock_t lock;
};

unsigned int reg_read(struct feserial_dev *dev, int off)
{
	return readl(dev->regs + 4*off);
}

void reg_write(struct feserial_dev *dev, unsigned int val, int off)
{
	writel(val, dev->regs + 4*off);
}

void uart_write(struct feserial_dev *dev, char c)
{
	while (!(reg_read(dev, UART_LSR) & UART_LSR_THRE))
		cpu_relax();

	reg_write(dev, c, UART_TX);
}

static irqreturn_t feserial_irq_handler(int irq, void *dev_id)
{
	char c;
	struct feserial_dev *dev = dev_id;
	unsigned long flags;

	spin_lock_irqsave(&dev->lock, flags);

	/* ACK the interrupt and store the received character. */
	do {
		c = reg_read(dev, UART_RX);
	} while (!(reg_read(dev, UART_IIR) & UART_IIR_NO_INT));
	dev->serial_buf[dev->serial_buf_wr] = c;
	pr_debug("IRQ read %c\n", c);

	/* Update write index. */
	dev->serial_buf_wr++;
	if (dev->serial_buf_wr == SERIAL_BUFSIZE)
		dev->serial_buf_wr = 0;

	spin_unlock_irqrestore(&dev->lock, flags);

	wake_up(&dev->wq);

	return IRQ_HANDLED;
}

static ssize_t feserial_read(struct file *file, char __user *buf, size_t count,
					loff_t *offset)
{
	struct feserial_dev *dev;
	int ret;
	unsigned long flags;

	dev = container_of(file->private_data, struct feserial_dev, miscdev);

	spin_lock_irqsave(&dev->lock, flags);
	if (dev->serial_buf_rd == dev->serial_buf_wr) {
		/*
		 * No more data available; sleep and wait. Drop the lock
		 * first so someone can fill in the data!
		 */
		spin_unlock_irqrestore(&dev->lock, flags);
		ret = wait_event_interruptible(dev->wq,
				dev->serial_buf_rd != dev->serial_buf_wr);
		/* Return if we were interrupted. */
		if (ret == -ERESTARTSYS) {
			return 0;
		}
		spin_lock_irqsave(&dev->lock, flags);
	}
	put_user(dev->serial_buf[dev->serial_buf_rd], buf);

	pr_debug("read %c\n", dev->serial_buf[dev->serial_buf_rd]);

	/* Update read index. */
	dev->serial_buf_rd++;
	if (dev->serial_buf_rd == SERIAL_BUFSIZE)
		dev->serial_buf_rd = 0;
	spin_unlock_irqrestore(&dev->lock, flags);

	return 1;
}

static ssize_t feserial_write(struct file *file, const char __user *buf,
					size_t count, loff_t *offset)
{
	char c;
	struct feserial_dev *dev;
	int i;
	unsigned long flags;

	dev = container_of(file->private_data, struct feserial_dev, miscdev);

	spin_lock_irqsave(&dev->lock, flags);

	for (i = 0; i < count; i++) {
		get_user(c, &buf[i]);
		pr_debug("wrote %c\n", c);
		uart_write(dev, c);
	}
	dev->write_count += count;

	spin_unlock_irqrestore(&dev->lock, flags);

	return count;
}

static long feserial_ioctl(struct file *file, unsigned int cmd,
				unsigned long arg)
{
	unsigned int __user *argp;
	struct feserial_dev *dev;
	unsigned long flags;
	long ret;

	dev = container_of(file->private_data, struct feserial_dev, miscdev);

	spin_lock_irqsave(&dev->lock, flags);

	switch(cmd) {
	case SERIAL_RESET_COUNTER:
		dev->write_count = 0;
		ret = 0;
		break;
	case SERIAL_GET_COUNTER:
		argp = (unsigned int __user *) arg;
		put_user(dev->write_count, argp);
		ret = 0;
		break;
	default:
		ret = -ENXIO;
		break;
	}

	spin_unlock_irqrestore(&dev->lock, flags);

	return ret;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = feserial_read,
	.write = feserial_write,
	.unlocked_ioctl = feserial_ioctl
};


static int feserial_probe(struct platform_device *pdev)
{
	unsigned int baud_divisor;
	struct feserial_dev *dev;
	struct dentry *file;
	struct resource *res;
	int status;
	unsigned int uartclk;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		pr_err("Cannot get resource\n");
		return -ENODEV;
	}

	dev = devm_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);
	if (dev == NULL) {
		dev_err(&pdev->dev, "Cannot allocate dev\n");
		return -ENOMEM;
	}

	dev->regs = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(dev->regs)) {
		dev_err(&pdev->dev, "Cannot remap registers\n");
		return PTR_ERR(dev->regs);
	}

	dev->irq = platform_get_irq(pdev, 0);
	if (dev->irq <= 0) {
		dev_err(&pdev->dev, "Unable to obtain IRQ\n");
		return -ENODEV;
	}
	status = devm_request_irq(&pdev->dev, dev->irq, feserial_irq_handler, 0, "feserial",
		dev);
	if (status < 0) {
		dev_err(&pdev->dev, "Unable to request IRQ\n");
		return status;
	}

	dev->write_count = 0;
	dev->serial_buf_rd = 0;
	dev->serial_buf_wr = 0;
	init_waitqueue_head(&dev->wq);
	spin_lock_init(&dev->lock);

	platform_set_drvdata(pdev, dev);

	pm_runtime_enable(&pdev->dev);
	pm_runtime_get_sync(&pdev->dev);

	/* Baud rate */
	of_property_read_u32(pdev->dev.of_node, "clock-frequency", &uartclk);
	baud_divisor = uartclk / 16 / 115200;
	reg_write(dev, 0x07, UART_OMAP_MDR1);
	reg_write(dev, 0x00, UART_LCR);
	reg_write(dev, UART_LCR_DLAB, UART_LCR);
	reg_write(dev, baud_divisor & 0xff, UART_DLL);
	reg_write(dev, (baud_divisor >> 8) & 0xff, UART_DLM);
	reg_write(dev, UART_LCR_WLEN8, UART_LCR);

	/* Enable receiver data interrupts. */
	reg_write(dev, UART_IER_RDI, UART_IER);

	/* Soft reset */
	reg_write(dev, UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT, UART_FCR);
	reg_write(dev, 0x00, UART_OMAP_MDR1);

	/* Setup misc device. */
	dev->miscdev.minor = MISC_DYNAMIC_MINOR;
	dev->miscdev.name = kasprintf(GFP_KERNEL, "feserial-%x", res->start);
	if (dev->miscdev.name == NULL) {
		status = -ENOMEM;
		goto error_pm_runtime;
	}

	dev->miscdev.fops = &fops;
	status = misc_register(&dev->miscdev);
	if (status < 0) {
		goto error_miscdev_name;
	}

	/* Setup debugfs entries. */
	dev->debugfs_parent = debugfs_create_dir(dev->miscdev.name, NULL);
	if (dev->debugfs_parent == NULL) {
		status = -ENOMEM;
		goto error_misc_register;
	}
	file = debugfs_create_u32("write_count", S_IRUGO, dev->debugfs_parent,
							&dev->write_count);
	if (file == NULL) {
		status = -ENOMEM;
		goto error_debugfs;
	}

	return 0;

error_debugfs:
	debugfs_remove_recursive(dev->debugfs_parent);
error_misc_register:
	misc_deregister(&dev->miscdev);
error_miscdev_name:
	kfree(dev->miscdev.name);
error_pm_runtime:
	pm_runtime_disable(&pdev->dev);
	return status;
}

static int feserial_remove(struct platform_device *pdev)
{
	struct feserial_dev *dev = platform_get_drvdata(pdev);

	misc_deregister(&dev->miscdev);
	kfree(dev->miscdev.name);
	debugfs_remove_recursive(dev->debugfs_parent);
	pm_runtime_disable(&pdev->dev);

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id feserial_of_match[] = {
		{ .compatible = "free-electrons,serial" },
		{}
};
MODULE_DEVICE_TABLE(of, feserial_of_match);
#endif

static struct platform_driver feserial_driver = {
	.driver = {
		.name = "feserial",
		.owner = THIS_MODULE,
		.of_match_table = feserial_of_match
	},
	.probe = feserial_probe,
	.remove = feserial_remove,
};

module_platform_driver(feserial_driver);
MODULE_LICENSE("GPL");