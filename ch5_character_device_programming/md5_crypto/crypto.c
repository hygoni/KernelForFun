#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/crypto.h>
#include <linux/mm.h>
#include <asm/uaccess.h>
#include <linux/scatterlist.h>

#define DEVICE_NAME "crypto"
#define MINOR_BASE 0
#define MAX_TAP 8
#define MAX_LEN 1024
#define MAX_ENCRYPTED_LEN 64
#define MAX_DIGEST_SIZE 64

static const char *device_name = "crypto";
static dev_t my_dev;
static struct cdev my_cdev;
static int size = 0;
static char *my_buf = NULL;

static char md5_result[MAX_LEN];
static atomic_t in_use = ATOMIC_INIT(1);

struct md5 {
	char plaintext[MAX_LEN + 1];
	char encrypted[MAX_ENCRYPTED_LEN + 1];
	unsigned int size;
};

static struct md5 md5_template;

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

	if (!atomic_dec_and_test(&in_use)) {
		atomic_inc(&in_use);
		return -EBUSY;
	}
	return 0;
}

static int hash_md5(struct md5 *m, char *buf, size_t size) {
	struct crypto_tfm *tfm;
	struct scatterlist sg[1];

	if ((tfm = crypto_alloc_tfm("md5", 0)) == NULL) {
		printk(KERN_ALERT "[%s] %s - failed to allocate tfm\n",
				DEVICE_NAME, __func__);
		return -1;
	}

	memset(m->plaintext, 0, MAX_ENCRYPTED_LEN + 1);
	crypto_digest_init(tfm);
	crypto_digest_update(tfm, sg, 1);
	crypto_digest_final(tfm, m->plaintext);
	m->size = crypto_tfm_alg_digest(tfm);
	return 0;
}

int device_release(struct inode *inode, struct file *filp) {
	printk(KERN_INFO "[%s] %s\n", DEVICE_NAME, __func__);

	atomic_inc(&in_use);
	return 0;
}

ssize_t device_read(struct file *filp, char *buf, size_t count, loff_t *fpos) {
	int copied;

	if (count > MAX_LEN)
		count = MAX_LEN;

	hash_md5(md5_result, &md5_template);
	copied = copy_to_user(md5_result, buf, count);
	return (count - copied);
}

ssize_t device_write(struct file *filp, const char *buf, size_t count, loff_t *fpos) {
	int copied;

	if (count > MAX_LEN) {
		return -EINVAL;
	}

	copied = copy_from_user(md5_template.plaintext, buf, count);
	md5_template.plaintext[count - copied] = '\0';
	md5_template.psize = count - copied;
	return md5_template.psize;
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

	if ((tfm = crypto_alloc_tfm("md5", 0)) == NULL) {
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


