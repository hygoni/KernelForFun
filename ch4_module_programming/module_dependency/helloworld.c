#include <linux/module.h>
#include <linux/kernel.h>

extern int add(int x, int y);

int __init helloworld_init(void)
{
	printk(KERN_DEBUG "Hello, World! 1 + 1 = %d!\n", add(1, 1));
	return 0;
}

void __exit helloworld_exit(void)
{
	printk(KERN_DEBUG "HelloWorld - made By hyeyoo\n");
}

module_init(helloworld_init);
module_exit(helloworld_exit);

MODULE_AUTHOR("Hyeonggon Yoo <42.hyeyoo@gmail.com>");
MODULE_DESCRIPTION("helloworld module using adder module");
MODULE_LICENSE("GPL");
