#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/moduleparam.h>

#define prk(x,args...) printk("jinmin,[%s,%d]--" x,__FUNCTION__,__LINE__,##args)
#define MAX 20
static char c_kbuf[MAX];

static struct completion com;
static int option=0;
static int complete_open (struct inode * inode, struct file *file)
{
	prk("open ok\n");
	return 0;
}

static ssize_t complete_read (struct file *file, char __user *buffer, size_t count, loff_t * ppos)
{
	loff_t pos=file->f_pos;
	/*prk("in read,count=%d,pos=%lld,*ppos=%lld\n",count,pos,*ppos);*/
	
	wait_for_completion_interruptible(&com);
	if(count>=MAX)
		count=MAX;
	
	if( copy_to_user((char __user *)buffer,c_kbuf+pos,count) )	
	{
		prk("cooy to user error\n");
		return -1;
	}

	/**ppos+=count;*/
	prk("after read,c_kbuf=%s,count=%d,pos=%lld\n",c_kbuf,count,file->f_pos);
	return count;
}



static ssize_t complete_write (struct file *file, const char __user *buffer, size_t count, loff_t * ppos)
{
	prk("in write,count=%d\n",count);
	if(count>=MAX)
		count=MAX;
	if( copy_from_user(c_kbuf,buffer,count) )
	{
		prk("cooy from user error\n");
		return -1;
	}
	prk("after write,c_kbuf=%s\n",c_kbuf);
	if(option==0)
		complete(&com);
	else
		complete_all(&com);
	return count;
}

static loff_t complete_llseek (struct file *file, loff_t offset, int whence)
{
	loff_t pos=file->f_pos;
	prk("in llseek\n");
	switch (whence)
	{
		case 0:
			 pos=offset;
			 break;
		case 1:
			 pos+=offset;
			 break;
		case 2:
			pos=MAX+offset-1;
			 break;
		default:
			 return -EINVAL;
	}
	if(pos>=MAX||pos<0)
		return -EINVAL;
	return file->f_pos=pos;
	
}
static struct file_operations fops_complete={
	.owner	=THIS_MODULE,
	.open	=	complete_open,
	.read	=	complete_read,
	.write	=	complete_write,
	.llseek	=	complete_llseek,
	
};

static struct miscdevice miscdev_complete = {
	.minor	=	MISC_DYNAMIC_MINOR,
	.name	=	"misc_complete",
	.fops	=	&fops_complete,
};



static int complete_init(void)
{
	
	prk("init\n");
	misc_register(&miscdev_complete);
	init_completion(&com);
	printk("S_IRUGO=%x,S_IWUSR=%d\n",S_IRUGO,S_IWUSR);
	printk("option=%d\n",option);
	return 0;
}

static void complete_exit(void)
{
	misc_deregister(&miscdev_complete);
	prk("exit");
}

module_init(complete_init);
module_exit(complete_exit);

module_param(option,int,S_IRUGO);




MODULE_LICENSE("GPL");
