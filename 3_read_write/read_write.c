#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/kernel.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mickael Bruzzi");
MODULE_DESCRIPTION("Register a device Number and implement some callback functions");

#define DRIVER_NAME "dummydriver"
#define DRIVER_CLASS "MyModuleClass"

/* Buffer for data */
static char buffer[255];
static size_t bufferPointer;

/* Variables for device and device class */
static dev_t myDeviceNbr;
static struct class *myClass;
static struct cdev myDevice;


/*
 * @brief: Read data out of the buffer
 */
static ssize_t driver_read(struct file *File, char *userBuffer, size_t count, loff_t *offs)
{
	int toCopy;
	int notCopied;
	int delta;

	/* Get amount of data to copy */
	toCopy = min(count, bufferPointer);

	/* Copy data to user */
	notCopied = copy_to_user(userBuffer, buffer, toCopy);

	/* Calculate data */
	delta = toCopy - notCopied;
	return (delta);
}

/*
 * @brief: Write data to buffer
 */
static ssize_t driver_write(struct file *File, const char *userBuffer, size_t count, loff_t *offs)
{
	int toCopy;
	int notCopied;
	int delta;

	/* Get amount of data to copy */
	toCopy = min(count, sizeof(buffer));

	/* Copy data to buffer */
	notCopied = copy_from_user(buffer, userBuffer, toCopy);
	bufferPointer = toCopy;

	/* Calculate data */
	delta = toCopy - notCopied;
	return (delta);
}


/*
 *  @brief: This function is called when the device file is opened
 *  */
static int driver_open(struct inode *deviceFile, struct file *instance)
{
	printk("dev_nbrs - open was called!\n");
	return (0);
}

/*
 *  @brief: This function is called when the device file is closed
 *  */
static int driver_close(struct inode *deviceFile, struct file *instance)
{
	printk("dev_nbrs - close was called!\n");
	return (0);
}

static struct file_operations fops = 
{
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};
/*
 *  @brief: This function is called when the module is loaded into the kernel
 */
static int __init moduleInit(void)
{	
	int retval;

	printk("Hello, Kernel!\n");

	/* Allocate device number */
	if (alloc_chrdev_region(&myDeviceNbr, 0, 1, DRIVER_NAME) < 0)
	{
		printk("Device nbr - Could not be allocated!\n");
		return (-1);
	}
	printk ("read-Write - Device nbr. Major: %d, Minor: %d, was registered!\n", MAJOR(myDeviceNbr), MINOR(myDeviceNbr));
	
	/* Create device class */
	if ((myClass = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL)
	{
		printk("Device class can not be created!\n");
		goto ClassError;
	}

	/* Create device file */
	if (device_create(myClass, NULL, myDeviceNbr, NULL, DRIVER_NAME) == NULL)
	{
		printk("Can't create device file");
		goto FileError;
	}

	/* Initialize device file */
	cdev_init(&myDevice, &fops);

	/*  Registering device to kernel */
	if (cdev_add(&myDevice, myDeviceNbr, 1) == -1)
	{
		printk("Registering of device to kernel failed!");
		goto AddError;
	}
	return (0);

AddError:
	device_destroy(myClass, myDeviceNbr);
FileError:
	class_destroy(myClass);
ClassError:
	unregister_chrdev(myDeviceNbr, DRIVER_NAME);
	return (-1);
}

/*
 *  @brief: This function is called when the module is removed from the kernel
 */

static void __exit moduleExit(void)
{
	cdev_del(&myDevice);
	device_destroy(myClass, myDeviceNbr);
	class_destroy(myClass);
	unregister_chrdev(myDeviceNbr, DRIVER_NAME);
	printk("Goodbye, Kernel!\n");
}

module_init(moduleInit);
module_exit(moduleExit);

