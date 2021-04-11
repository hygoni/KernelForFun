#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/time.h>

#define MODULE_NAME "TIMER"

int __init timer_init(void) {
	struct timespec64 ts;
	unsigned long delay;

	ktime_get_ts64(&ts);
	printk(KERN_INFO "[%s] %s currnt time: %llu\n", MODULE_NAME, __func__, ts.tv_sec);
	printk(KERN_INFO "[%s] %s waiting 3 seconds...\n", MODULE_NAME, __func__);

	delay = jiffies + 3 * HZ;
	while (time_before(jiffies, delay))
		_cond_resched();
	ktime_get_ts64(&ts);
	printk(KERN_INFO "[%s] %s currnt time: %llu\n", MODULE_NAME, __func__, ts.tv_sec);
	return 0;
}

void __exit timer_exit(void) {
	
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
