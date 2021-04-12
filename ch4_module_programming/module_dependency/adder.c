#include <linux/module.h>
#include <linux/kernel.h>

int __init adder_init(void)
{
	printk(KERN_DEBUG "loading adder\n");
	return 0;
}

void __exit adder_exit(void)
{
	printk(KERN_DEBUG "unloading adder\n");
}

int add(int x, int y)
{
	return x + y;
}
EXPORT_SYMBOL(add);

module_init(adder_init);
module_exit(adder_exit);

MODULE_AUTHOR("Hyeonggon Yoo <42.hyeyoo@gmail.com>");
MODULE_DESCRIPTION("adder module for helloworld module");
MODULE_LICENSE("GPL");
