#include <linux/module.h>
#include <linux/timer.h>
#include <linux/atomic.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

#define prk(x,args...) printk("jinmin,[%s,%d] "x,__FUNCTION__,__LINE__,##args)
typedef struct mtimedev{
	struct work_struct work;
	/*atomic_t count;*/
	int count;
	struct timer_list *timer;
}timerdev,*ptimerdev;

static struct timer_list timer;
struct mtimedev dev;

static struct workqueue_struct *time_workqueue;
static struct work_struct time_work;


static void time_queue_work(struct work_struct * work)
{
	/*atomic_inc(&dev.count);*/
	dev.count++;
	/*prk("count=%d\n",dev.count.counter);*/
	prk("count=%d\n",dev.count);
	
}

void time_fuc(unsigned long data)
{
	
	mod_timer(dev.timer,jiffies+HZ);	
	queue_work(time_workqueue,&time_work);
}

static int time_init(void)
{
	/*dev.timer=kmalloc(sizeof(struct timer_list),GFP_KERNEL);*/
	dev.timer=&timer;
	init_timer(dev.timer);	
	dev.timer->function=time_fuc;
	dev.timer->expires=jiffies+HZ;
	add_timer(dev.timer);
		
	time_workqueue=create_singlethread_workqueue("time_work");
	INIT_WORK(&time_work,time_queue_work);
	return 0;
}

static void time_exit(void)
{
	del_timer_sync(dev.timer);
	/*kfree(dev.timer);*/
	flush_workqueue(time_workqueue);
	destroy_workqueue(time_workqueue);
}

module_init(time_init);
module_exit(time_exit);
MODULE_LICENSE("GPL");

