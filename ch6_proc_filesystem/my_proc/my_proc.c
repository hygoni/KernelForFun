#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/seq_file.h>

#define MAX_DATA_LEN 4096
#define DEVICE_NAME "my_proc"

struct my_data {
	char *buf;
	int size;
};

static struct my_data data;
static struct proc_dir_entry *my_proc = NULL;

ssize_t device_read(struct file *filp, char *buf, size_t count, loff_t *f_pos) {
	int copied;

	if (count > data.size)
		count = data.size;

	copied = copy_to_user(data.buf, buf, count);
	return (count - copied);
}

ssize_t device_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
	int copied;

	if (count > MAX_DATA_LEN)
		count = MAX_DATA_LEN;

	copied = copy_from_user(data.buf, buf, count);
	data.buf[count - copied] = '\0';
	data.size = count - copied;
	return (count - copied);
}

int device_show(struct seq_file *m, void *v) {
	seq_printf(m, "%s", data.buf);
	
	return 0;
}

int device_open(struct inode *inode, struct file *file) {
	return single_open(file, device_show, NULL);
}

static const struct proc_ops fops = {
	.proc_read = seq_read,
	.proc_open = device_open,
	.proc_write = device_write,
	.proc_release = single_release,
	.proc_lseek = seq_lseek,
};

int __init proc_init(void) {
	if ((data.buf = (char*)kmalloc(MAX_DATA_LEN + 1, GFP_KERNEL)) == NULL) {
		printk(KERN_ALERT "[%s] %s - kmalloc failed\n",
				DEVICE_NAME, __func__);
		return -ENOMEM;
	}
	data.size = 0;

	if ((my_proc = proc_create(DEVICE_NAME, 0666, NULL, &fops)) == NULL) {
		printk(KERN_ALERT "[%s] %s - proc_create failed\n",
				DEVICE_NAME, __func__);
		return -ENOMEM;
	}

	printk(KERN_ALERT "[%s] %s - successfully loaded!\n",
			DEVICE_NAME, __func__);
	return 0;
}

void __exit proc_exit(void) {
	remove_proc_entry(DEVICE_NAME, NULL);
	proc_remove(my_proc);

	if (data.buf != NULL)
		kfree(data.buf);
	printk(KERN_ALERT "[%s] %s - successfully unloaded!\n",
			DEVICE_NAME, __func__);
}

module_init(proc_init);
module_exit(proc_exit);
MODULE_LICENSE("GPL");
