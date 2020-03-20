/* Demo use of_API for gets data from Device Tree
 * Author: Nguyen Trong Dung <nguyentrongdung0498@gmail.com
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>

struct gpio_t {
	void __iomem *base;
	int led_pin;
};

static int my_probe(struct platform_device *pdev)
{
	char ret = 0;
	struct gpio_t *led;
	struct resource *res = NULL;
	struct device_node *np = pdev->dev.of_node;

	led = devm_kzalloc(&pdev->dev, sizeof(*led), GFP_KERNEL);
	if (led == NULL) {
		pr_info("Can not allocate memmory\n");
		return -ENOMEM;
	}

	/* Just get resource for demo, not mapping with VMM.
	 * If mapping will gets error, because conlict with region
	 * that is previous mapping.
	 */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		pr_info("Can not get resource from device tree.\n");
		goto resource_not_exist;
	}
	pr_info("Get address: %x from DT\n", res->start);

	/* Demo get gpio number from phandle of gpio.
	 * This number can use with APIs in linux/gpio.h
	 */
	led->led_pin = of_get_named_gpio(np, "demo_led", 0);
	pr_info("Get gpio number: %d from DT\n", led->led_pin);

	if (!gpio_is_valid(led->led_pin)) {
		pr_err("gpio pin %d is not available\n", led->led_pin);
		goto resource_not_exist;
	}

	ret = gpio_request(led->led_pin, "gpio_115");
	if (ret) {
		pr_err("Can not request gpio: %d\n", led->led_pin);
		goto resource_not_exist;
	}

	gpio_direction_output(led->led_pin, 1);
	gpio_set_value(led->led_pin, 1);

	/* Set driver data for pdev, which used in remove function */
	platform_set_drvdata(pdev, led);
	return 0;

resource_not_exist:
	kfree(led);
	return -1;
}

static int my_remove(struct platform_device *pdev)
{
	/* Get driver data which is set by probe function */
	struct gpio_t *led = platform_get_drvdata(pdev);

	gpio_set_value(led->led_pin, 0);
	gpio_free(led->led_pin);

	devm_kfree(&pdev->dev, led);

	pr_emerg("Dungnt98 %s, %d\n", __func__, __LINE__);
	return 0;
}

struct of_device_id dungnt98_of_match[] = {
	{
		.compatible = "dungnt98, demo_dts"
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

MODULE_AUTHOR("Trong Dung <nguyentrongdung0498@gmail.com>");
MODULE_LICENSE("GPL");
