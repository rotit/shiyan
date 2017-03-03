#include <linux/module.h>
#include <linux/timer.h>
#include <linux/atomic.h>
#include <linux/slab.h>

#define prk(x,args...) printk("jinmin,[%s,%d] "x,__FUNCTION__,__LINE__,##args)
static struct timer_list timer;
typedef struct mtimedev{
	/*atomic_t count;*/
	int count;
	struct timer_list *timer;
}timerdev,*ptimerdev;

struct mtimedev dev;

void time_fuc(unsigned long data)
{
	
	/*atomic_inc(&dev.count);*/
	dev.count++;
	/*prk("count=%d\n",dev.count.counter);*/
	prk("count=%d\n",dev.count);
	mod_timer(dev.timer,jiffies+HZ);	
}

static int time_init(void)
{
	/*dev.timer=kmalloc(sizeof(struct timer_list),GFP_KERNEL);*/
	dev.timer=&timer;
	init_timer(dev.timer);	
	dev.timer->function=time_fuc;
	dev.timer->expires=jiffies+HZ;
	add_timer(dev.timer);
		
	return 0;
}

static void time_exit(void)
{
	del_timer_sync(dev.timer);
	/*kfree(dev.timer);*/
}

module_init(time_init);
module_exit(time_exit);

