#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xa40d049c, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xac0bad95, __VMLINUX_SYMBOL_STR(platform_driver_unregister) },
	{ 0xeabe6ac7, __VMLINUX_SYMBOL_STR(__platform_driver_register) },
	{ 0xedc03953, __VMLINUX_SYMBOL_STR(iounmap) },
	{ 0x79c5a9f0, __VMLINUX_SYMBOL_STR(ioremap) },
	{ 0x7432a79a, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0xb21a09a0, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0xf6243e26, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x95aa0696, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0x3614848b, __VMLINUX_SYMBOL_STR(devm_ioremap_resource) },
	{ 0x29de7e8, __VMLINUX_SYMBOL_STR(platform_get_resource) },
	{ 0x422196b2, __VMLINUX_SYMBOL_STR(devm_kmalloc) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x3912e57c, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x1dd5e400, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x62e5eaa2, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0x14695d7d, __VMLINUX_SYMBOL_STR(devm_iounmap) },
	{ 0x822137e2, __VMLINUX_SYMBOL_STR(arm_heavy_mb) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("of:N*T*Cdungnt98,gpio2_2");
MODULE_ALIAS("of:N*T*Cdungnt98,gpio2_2C*");
