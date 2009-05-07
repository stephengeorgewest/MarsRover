/* Necessary includes for device drivers */

// do sudo or as root for make, insmod, rmmod
// "lsmod" shows the modules in the kernel
// use "dmesg"   to show the messages from the log
// "mknod /dev/coffee c MAJOR MINOR" ---------- this makes the file to read from for the device where MAJOR is the major number, c is for char device, MINOR is minor number
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>

#include <linux/fcntl.h> /* O_ACCMODE */
#include <asm/system.h> /* cli(), *_flags */
#include <asm/uaccess.h> /* copy_from/to_user */
#include <linux/string.h>

#define DEVICE_NAME "DeviceDriverTest"
#define SUCCESS 0
#define BUF_LEN 80

MODULE_LICENSE("Dual BSD/GPL");

/* Declaration of device.c functions */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static void device_exit(void);
static int device_init(void);

/* Structure that declares the usual file */
/* access functions */
static struct file_operations fops = {
  read: device_read,
  write: device_write,
  open: device_open,
  release: device_release
};


/* Declaration of the init and exit functions */
module_init(device_init);
module_exit(device_exit);

/* Global variables of the driver */
static int Major;		/* Major number assigned to our device driver */
static int Device_Open = 0;	/* Is device open?  
				 * Used to prevent multiple access to device */
static char msg[BUF_LEN];	/* The msg the device will give when asked */
static char *msg_Ptr;

static int count[5];

static int device_init(void)
{
	count[0] = 125;
	count[1] = 29;
	count[2] = 1523;
	count[3] = 45;
	count[4] = 12321;
	
 	Major = register_chrdev(0, DEVICE_NAME, &fops);

	if (Major < 0) 
	{
	  printk(KERN_ALERT "Registering char device failed with %d\n", Major);
	  return Major;
	}

	printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
	printk(KERN_INFO "the driver, create a dev file with\n");
	printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
	printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
	printk(KERN_INFO "the device file.\n");
	printk(KERN_INFO "Remove the device file and module when done.\n");
	
	return 0;
}

static void device_exit(void)
{
	unregister_chrdev(Major, DEVICE_NAME);
}

static int device_open(struct inode *inode, struct file *file)
{
	static int counter = 0;

	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	sprintf(msg, "%d %d %d %d %d\n", count[0],count[1],count[2],count[3],count[4]);
	msg_Ptr = msg;
	printk(KERN_ALERT "%s\n",msg);
	//msg_Ptr = (char*)angles;
	try_module_get(THIS_MODULE);

	return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
	Device_Open--;		/* We're now ready for our next caller */

	/* 
	 * Decrement the usage count, or else once you opened the file, you'll
	 * never get get rid of the module. 
	 */
	module_put(THIS_MODULE);

	return 0;
}

static ssize_t device_read(struct file *filp,	/* see include/linux/fs.h   */
			   char *buffer,	/* buffer to fill with data */
			   size_t length,	/* length of the buffer     */
			   loff_t * offset)
{
	//memcpy(msg, angles, 20); //copy 20 bytes starting at angles address(so 5 floats of 4 bytes) into address starting at msg
	/*
	 * Number of bytes actually written to the buffer 
	 */
	int bytes_read = 0;
	//memcpy(msg, angles, 20); //copy 20 bytes starting at angles address(so 5 floats of 4 bytes) into address starting at msg
	
	/*
	 * If we're at the end of the message, 
	 * return 0 signifying end of file 
	 */
	if (*msg_Ptr == 0)
		return 0;

	/* 
	 * Actually put the data into the buffer 
	 */
	
	while (length && *msg_Ptr) 
	{

		/* 
		 * The buffer is in the user data segment, not the kernel 
		 * segment so "*" assignment won't work.  We have to use 
		 * put_user which copies data from the kernel data segment to
		 * the user data segment. 
		 */
		
		put_user(*(msg_Ptr++), buffer++);
		//put_user(*(angles),buffer++);
		length--;
		bytes_read++;
	}

	/* 
	 * Most read functions return the number of bytes put into the buffer
	 */
	return bytes_read;
}

static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	printk(KERN_ALERT "Sorry, this operation isn't supported.\n");
	return -EINVAL;
}



