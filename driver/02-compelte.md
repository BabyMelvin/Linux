内核编程中常见的一种模式是，**在当前线程之外初始化某个活动**，**然后等待该活动的结束**。

这个活动可能是，创建一个`新的内核线程`或者`新的用户空间进程`、对一个已有进程的某个请求，或者某种类型的硬件动作，等等。在这种情况下，我们可以使用`信号量`来同步这两个任务。然而，内核中提供了另外一种机制——completion接口。**Completion是一种轻量级的机制**，他允许**一个线程**告诉**另一个线程**某个工作已经完成。

# 1.结构与初始化
 Completion在内核中的实现基于等待队列，completion结构很简单：

```c
struct completion {
	unsigned int done;     /*用于同步的原子量*/
	wait_queue_head_t wait;/*等待事件队列*/
};
```

和信号量一样，初始化分为静态初始化和动态初始化两种情况：

* 静态初始化：

```
#define COMPLETION_INITIALIZER(work) \
	{ 0, __WAIT_QUEUE_HEAD_INITIALIZER((work).wait) }
 
#define DECLARE_COMPLETION(work) \
	struct completion work = COMPLETION_INITIALIZER(work)
```

* 动态初始化：

```c
static inline void init_completion(struct completion *x)
{
	x->done = 0;
	init_waitqueue_head(&x->wait);
}
```
可见，两种初始化都将用于同步的done原子量置位了0，后面我们会看到:

* 该变量在`wait`相关函数中**减一**
* 在`complete`系列函数中**加一**。

# 2.实现
同步函数一般都成对出现，completion也不例外，我们看看最基本的两个`complete`和`wait_for_completion`函数的实现。

* `wait_for_completion`最终由下面函数实现：

```c
static inline long __sched
do_wait_for_common(struct completion *x, long timeout, int state)
{
	if (!x->done) {
		DECLARE_WAITQUEUE(wait, current);
 
		wait.flags |= WQ_FLAG_EXCLUSIVE;
		__add_wait_queue_tail(&x->wait, &wait);
		do {
			if (signal_pending_state(state, current)) {
				timeout = -ERESTARTSYS;
				break;
			}
			__set_current_state(state);
			spin_unlock_irq(&x->wait.lock);
			timeout = schedule_timeout(timeout);
			spin_lock_irq(&x->wait.lock);
		} while (!x->done && timeout);
		__remove_wait_queue(&x->wait, &wait);
		if (!x->done)
			return timeout;
	}
	x->done--;
	return timeout ?: 1;
}
```

* 而`complete`实现如下：

```c
void complete(struct completion *x)
{
	unsigned long flags;
 
	spin_lock_irqsave(&x->wait.lock, flags);
	x->done++;
	__wake_up_common(&x->wait, TASK_NORMAL, 1, 0, NULL);
	spin_unlock_irqrestore(&x->wait.lock, flags);
}
```
不看内核实现的源代码我们也能想到他的实现，不外乎在wait函数中循环等待done变为可用（正），而另一边的complete函数为唤醒函数，当然是将done加一，唤醒待处理的函数。是的，从上面的代码看到，和我们想的一样。内核也是这样做的。

# 运用

运用LDD3中的例子：

```c
#include <linux/module.h>
#include <linux/init.h>
 
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/completion.h>
 
MODULE_LICENSE("GPL");
 
static int complete_major=250;
DECLARE_COMPLETION(comp);

ssize_t complete_read(struct file *filp,char __user *buf,size_t count,loff_t *pos)
{
	printk(KERN_ERR "process %i (%s) going to sleep\n",current->pid,current->comm);
	wait_for_completion(&comp);
	printk(KERN_ERR "awoken %i (%s)\n",current-pid,current->comm);
	return 0;
}
ssize_t complete_write(struct file *filp,const char __user *buf,size_t count,loff_t *pos)
{
	printk(KERN_ERR "process %i (%s) awakening the readers ...\n",current->pid,current->comm);
	complete(&comp);
	return count;
}
struct file_operations complete_fops={
	.owner=THIS_MODULE,
	.read=complete_read,
	.write=complete_write,
};

int complete_init(void){
	int result;
	result=register_chrdev(complete_major,"complete",&complete_fops);
	if(result<0){
		return result;
	}
	if(complete_major == 0){
		complete_major = result;
	}
	return 0;
}
void complete_cleanup(void){
}
moudle_init(complete_init);
module_exit(complete_cleanup);
```

## 测试步骤

* 1.`mknod /dev/complete`创建complete节点，在linux上驱动程序需要手动创建文件节点
* 2.`insmod complete.ko`插入驱动模块，这里要注意的是，因为我们的代码中是手动分配的设备号，很可能被系统已经使用了，所以如果出现这种情况，查看`/proc/devices`文件。找一个没有被使用的设备号。
* 3.`cat /dev/complete`用于读该设备，调用设备的读函数
* 4.打开另一个终端输入`echo “hello” > /dev/complete`该命令用于写入该设备。