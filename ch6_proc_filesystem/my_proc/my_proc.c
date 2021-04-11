#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/seq_file.h>

#define MAX_DATA_LEN 4096
#define DEVICE_NAME "my_proc"

int data[] = {1, 2, 3, 4, 5};
const int size = 5;

static struct proc_dir_entry *my_proc = NULL;

void *device_start(struct seq_file *s, loff_t *pos) {
	printk(KERN_INFO "[%s] %s - pos = %lld\n",
			DEVICE_NAME, __func__, *pos);
	if (*pos >= size) {
		printk(KERN_INFO "[%s] %s - we're finished\n", DEVICE_NAME,
				__func__);
		return NULL;
	}

	return data + *pos;
}

void device_stop(struct seq_file *s, void *v) {

}

void *device_next(struct seq_file *s, void *v, loff_t *pos) {
	printk(KERN_INFO "[%s] %s - pos = %lld", DEVICE_NAME, __func__,
			*pos);
	(*pos)++;
	if (*pos >= size) {
		return NULL;
	}
	printk("[%s] %s - returned data: %d\n", DEVICE_NAME, __func__,
			(data[*pos]));
	return data + *pos;
}

/* v is what returned by next */
int device_show(struct seq_file *m, void *v) {
	printk(KERN_INFO "[%s] - %s printed %d\n", DEVICE_NAME, __func__,
			*(int*)v);
	seq_printf(m, "%d\n", *(int*)v);
	return 0;
}


static const struct seq_operations s_ops = {
	.start = device_start,
	.next = device_next,
	.stop = device_stop,
	.show = device_show
};

int device_open(struct inode *inode, struct file *file) {
	/* open using seq_file interface */
	return seq_open(file, &s_ops);
}

static const struct proc_ops p_ops = {
	/* read, lseek, release is processed by seq_file interface */
	.proc_read = seq_read,
	.proc_open = device_open,
	.proc_release = seq_release,
	.proc_lseek = seq_lseek,
};

int __init proc_init(void) {
	if ((my_proc = proc_create(DEVICE_NAME, 0666, NULL, &p_ops)) == NULL) {
		printk(KERN_ALERT "[%s] %s - proc_create failed\n",
				DEVICE_NAME, __func__);
		return -ENOMEM;
	}

	printk(KERN_ALERT "[%s] %s - successfully loaded!\n",
			DEVICE_NAME, __func__);
	return 0;
}

void __exit proc_exit(void) {
	proc_remove(my_proc);
	printk(KERN_ALERT "[%s] %s - successfully unloaded!\n",
			DEVICE_NAME, __func__);
}

module_init(proc_init);
module_exit(proc_exit);
MODULE_LICENSE("GPL");
