// compile module by doing:  make -C /usr/src/kernel-source-2. -.- M=pwd modules
//  load module by typing :  insmod DeviceDriverTest.ko
// lsmod lets you look at modules
// to remove module(uninstall) type: rmmod DeviceDriverTest 

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

static int DeviceDriverTest_init(void)
{
	printk("<1> Hello world!\n");
	return 0;
}

static void DeviceDriverTest_exit(void)
{
	printk("<1> Bye, cruel world\n");
}

module_init(DeviceDriverTest_init);
module_exit(DeviceDriverTest_exit);