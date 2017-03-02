#include <linux/module.h>
static void work_handler_me(struct work_struct *work);
DECLARE_WORK(my_work,work_handler_me);

struct workqueue_struct *my_wq;

static void work_handler_me(struct work_struct *work)
{
	printk("work_struct doooooo\n");
}


static int __init jin_init(void)
{
	int ret=0;
	my_wq=create_workqueue("my_queue");
	/*
	 *module init, it should return follow 0/-E convention
	 *
	 */
	ret=queue_work(my_wq,&my_work);//return 1,

	printk("jin ret=%d\n",ret);
	return 0;//ret;
}
static void __exit jin_exit(void )
{
	flush_workqueue(my_wq);
	destroy_workqueue(my_wq);
	printk("exit\n");
}

module_init(jin_init);
module_exit(jin_exit);
MODULE_LICENSE("GPL");
