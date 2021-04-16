/*
 * driver.c
 * This is buffer character device
 * write - save buffer to device buffer
 * read - read device buffer
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/cdev.h>

#define MINOR_BASE 0 /* starting number of minor number */
#define DEVICE_NAME "helloworld"

static dev_t my_dev;
static struct class *my_class;
static struct cdev my_cdev;

static int size = 0;
static char *device_buf = NULL;

int device_open(struct inode *inode, struct file *filp);
int device_release(struct inode *inode, struct file *filp);
ssize_t device_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
ssize_t device_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

/* overriding functions of Virtual File System,
 * used C99 feature
 */
static struct file_operations fops = {
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};

/* when open() is called */
int device_open(struct inode *inode, struct file *filp) {
	printk(KERN_INFO "hyeyoo device open\n");
	return 0;
}

/* when close() is called */
int device_release(struct inode *inode, struct file *filp) {
	printk(KERN_INFO "hyeyoo device release\n");
	return 0;
}

/* Copy userspace buffer to kernel buffer
 */
ssize_t device_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
	int copied;

	if (device_buf != NULL)
		kfree(device_buf);

	if ((device_buf = kmalloc(count + 1, GFP_KERNEL)) == NULL)
		return -1;

	size = count;
	copied = copy_from_user(device_buf, buf, count);
	printk("[%s] copied count = %ld, size = %u\n", __func__, count, copied);
	return count - copied;
}

/* copy kernel buffer to userspace buffer, saved by write */
ssize_t device_read(struct file *filp, char *buf, size_t count, loff_t *fpos) {
	int copied;

	if (device_buf == NULL)
		return -1;

	if (count > size)
		count = size;

	copied = copy_to_user(buf, device_buf, count);
	printk("[%s] copied count = %ld, size = %u\n", __func__, count, size);
	return (count - copied);
}

int	__init device_init(void) {
	printk(KERN_INFO "[%s]\n", __func__);
	/* try allocating character device */
	if (alloc_chrdev_region(&my_dev, MINOR_BASE, 1, DEVICE_NAME)) {
		printk(KERN_ALERT "[%s] alloc_chrdev_region failed\n", __func__);
		goto err_return;
	}

	/* init cdev */
	cdev_init(&my_cdev, &fops);

	/* add cdev */
	if (cdev_add(&my_cdev, my_dev, 1)) {
		printk(KERN_ALERT "[%s] cdev_add failed\n", __func__);
		goto unreg_device;
	}

	if ((my_class = class_create(THIS_MODULE, "example")) == NULL) {
		printk(KERN_ALERT "[%s] class_add failed\n", __func__);
		goto unreg_device;
	}

	if (device_create(my_class, NULL, my_dev, NULL, "example") == NULL) {
		goto unreg_class;
	}

	printk("[%s] successfully created device: Major = %d, Minor = %d",
			__func__, MAJOR(my_dev), MINOR(my_dev));
	return 0;

unreg_class:
	class_destroy(my_class);

unreg_device:
	unregister_chrdev_region(my_dev, 1);

err_return:
	return -1;
}

void __exit	device_exit(void) {
	device_destroy(my_class, my_dev);
	class_destroy(my_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(my_dev, 1);
	if (device_buf != NULL)
		kfree(device_buf);
}

module_init(device_init);
module_exit(device_exit);

MODULE_AUTHOR("hyeyoo");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("My first character device driver");
