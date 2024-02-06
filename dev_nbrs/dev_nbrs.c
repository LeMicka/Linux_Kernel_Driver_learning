#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mickael Bruzzi");
MODULE_DESCRIPTION("Register a device Number and implement some callback functions");

#define MYMAJOR 90

/*
 *  @brief: This function is called when the device file is opened
 *  */
static int driver_open(struct inode *device_file, struct file *instance)
{
	printk("dev_nbrs - open was called!\n");
	return (0);
}

/*
 *  @brief: This function is called when the device file is closed
 *  */
static int driver_close(struct inode *device_file, struct file *instance)
{
	printk("dev_nbrs - close was called!\n");
	return (0);
}

static struct file_operations fops = 
{
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close
};
/*
 *  @brief: This function is called when the module is loaded into the kernel
 */
static int __init moduleInit(void)
{	
	int retval;

	printk("Hello, Kernel!\n");
	/* register device number.*/
	retval = register_chrdev(MYMAJOR, "my_dev_nbr", &fops);
	if (retval == 0)
		printk("dev_nbrs - registered device number, major: %d, minor: %d\n", MYMAJOR, 0);
	else if (retval > 0)
		printk("dev_nbrs - registered devive number, major: %d, minor: %d\n", MAJOR(retval), retval&0xfffff);
	else
	{
		printk("Could not register device number!\n");
		return (-1);
	}

	return (0);
}

/*
 *  @brief: This function is called when the module is removed from the kernel
 */

static void __exit moduleExit(void)
{
	unregister_chrdev(MYMAJOR, "my_dev_nbr");
	printk("Goodbye, Kernel!\n");
}

module_init(moduleInit);
module_exit(moduleExit);

