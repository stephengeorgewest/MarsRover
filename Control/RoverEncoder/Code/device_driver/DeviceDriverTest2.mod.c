#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x96d03e4a, "struct_module" },
	{ 0x518eb764, "per_cpu__cpu_number" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0x7ea4629c, "module_put" },
	{ 0xb72397d5, "printk" },
	{ 0xa955b72c, "register_chrdev" },
	{ 0x9ef749e2, "unregister_chrdev" },
	{ 0xc3aaf0a9, "__put_user_1" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "C4A836B30C4B1EB6052350A");
