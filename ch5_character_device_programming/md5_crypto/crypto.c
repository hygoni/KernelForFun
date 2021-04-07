#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#define DEVICE_NAME "crypto"
#define MINOR_BASE 0

static const char *device_name = "crypto";
static dev_t my_dev;
static struct cdev my_cdev;
static int size = 0;
static char *my_buf = NULL;

int device_open(struct inode *inode, struct file *filp);
int device_release(struct inode *inode, struct file *filp);
ssize_t device_read(struct file *filp, char *buf, size_t count, loff_t *fpos);
ssize_t device_write(struct file *filp, const char *buf, size_t count, loff_t *fpos);

static struct file_operations fops = {
	.open = device_open,
	.release = device_release,
	.read = device_read,
	.write = device_write
};

int device_open(struct inode *inode, struct file *filp) {
	printk(KERN_INFO "[%s] %s\n", DEVICE_NAME, __func__);
	return 0;
}

int device_release(struct inode *inode, struct file *filp) {
	printk(KERN_INFO "[%s] %s\n", DEVICE_NAME, __func__);
	return 0;
}

ssize_t device_read(struct file *filp, char *buf, size_t count, loff_t *fpos) {

}

ssize_t device_write(struct file *filp, const char *buf, size_t count, loff_t *fpos) {

}

int __init device_init(void) {
	printk(KERN_INFO "[%s] %s\n", DEVICE_NAME, __func__);

	if (alloc_chrdev_region(&my_dev, MINOR_BASE, 1, DEVICE_NAME)) {
		printk(KERN_INFO "[%s] %s - alloc_chrdev_region failed\n",
				DEVICE_NAME, __func__);
		return -1;
	}

	cdev_init(&my_cdev, &fops);

	if (cdev_add(&my_cdev, my_dev, 1)) {
		printk(KERN_INFO "[%s] %s - alloc_chrdev_region failed\n",
				DEVICE_NAME, __func__);
		return -1;
	}

	printk(KERN_INFO "[%s] %s - successfully created major=%d, minor=%d\n",
			DEVICE_NAME, __func__, MAJOR(my_dev), MINOR(my_dev));
	return 0;

}

void __exit device_exit(void) {
	printk(KERN_INFO "[%s] %s\n", DEVICE_NAME, __func__);
	unregister_chrdev_region(my_dev, 1);
}


