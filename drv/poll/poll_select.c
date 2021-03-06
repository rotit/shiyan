#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/poll.h>
//  定义设备文件名
#define DEVICE_NAME "poll_select"
#define MAX_BUFFER_SIZE 20

static char buffer[MAX_BUFFER_SIZE];
static int buffer_char_count = 2;
static wait_queue_head_t my_queue;
struct semaphore sem;

static ssize_t demo_read(struct file *file, char __user *buf, size_t count,
		loff_t *ppos)
{
	ssize_t result = 0;

	printk("jinmin--- read count=%d\n",buffer_char_count);

	if (buffer_char_count > 0)
	{
		if(down_interruptible(&sem))
			return -ERESTARTSYS;
		if (copy_to_user(buf, buffer, buffer_char_count))
		{
			return -EINVAL;
		}
		buffer_char_count=0;
		wake_up_interruptible(&my_queue);
	printk("jinmin---after read count=%d\n",buffer_char_count);
		up(&sem);
		return result;
	}
	else
	{
		return 0;
	}


	return result;
}

static ssize_t demo_write(struct file *file, const char __user *buf,
		size_t count, loff_t *ppos)
{
	ssize_t result = 0;
	if(down_interruptible(&sem))
		return -ERESTARTSYS;
	printk("jinmin--- write count=%d\n",buffer_char_count);
	wait_event_interruptible(my_queue, buffer_char_count == 0);
	if (copy_from_user(buffer, buf, count))
	{
		printk("jinmin copy from user error\n");
		return -EINVAL;
	}
	result = count;
	buffer_char_count = count;
	printk("jinmin--- after write count=%d\n",buffer_char_count);
	up(&sem);
	return result;
}
unsigned int demo_poll(struct file *filp, struct poll_table_struct *wait)
{
	unsigned int mask = 0;
	printk("in driver poll\n");
	if(down_interruptible(&sem))
		return -ERESTARTSYS;

	poll_wait(filp, &my_queue, wait);
	// read
	if(buffer_char_count > 0)
	{
		mask |= (POLLIN | POLLRDNORM);
	}
	//  write
	if(buffer_char_count == 0)
	{
		mask |= POLLOUT | POLLWRNORM;
	}

	up(&sem);
    return mask;
}
static struct file_operations dev_fops =
{ .owner = THIS_MODULE, .read = demo_read, .write = demo_write, .poll=demo_poll };

static struct miscdevice misc =
{ .minor = MISC_DYNAMIC_MINOR, .name = DEVICE_NAME, .fops = &dev_fops };

//  初始化Linux驱动
static int __init demo_init(void)
{

	//  建立设备文件
	int ret = misc_register(&misc);
	init_waitqueue_head(&my_queue);
	sema_init(&sem, 1);
	printk("demo_init_success\n");

	return ret;
}

// 卸载Linux驱动
static void __exit demo_exit(void)
{
	printk("demo_exit_success\n");
	//  删除设备文件
	misc_deregister(&misc);
}
//  注册初始化Linux驱动的函数
module_init( demo_init);
//  注册卸载Linux驱动的函数
module_exit( demo_exit);
MODULE_LICENSE("GPL");
