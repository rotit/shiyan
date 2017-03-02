#include <linux/module.h>


static void work_handler_me(struct work_struct *work)
{
	printk("work_struct doooooo\n");
}

DECLARE_WORK(wqq,work_handler_me);

static int __init jin_init(void)
{
	schedule_work(&wqq);
	return 0;
}
static void __exit jin_exit(void )
{
	printk("exit\n");
	/*flush_workqueue(&wqq);*/
	/*destroy_workqueue(&wqq);*/
	123
}

adb
module_init(jin_init);
module_exit(jin_exit);
MODULE_LICENSE("GPL");
