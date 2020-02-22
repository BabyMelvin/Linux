# 字符 设备驱动-POLL 机制
poll 机制分析：所有的系统调用，基于都可以在它的名字前加上“sys_”前缀，这就是它在内核中对应的函数。比如系统调用open、read、write、poll，与之对应的内核函数为：sys_open、sys_read、sys_write、sys_poll。

# 一、内核框架
对于系统调用poll 或select，它们对应的内核函数都是sys_poll。分析sys_poll，即可理解poll机制。

sys_poll 函数位于fs/select.c 文件中，代码如下

```c
asmlinkage long sys_poll(struct pollfd __user *ufsd, unsigned int nfds,
	long timeout_msecs)
{
	s64 timeout_jiffies;
	if(timeout_msecs > 0) {
#if HZ > 1000
	// we can only overflow if HZ > 1000
	if(timeout_msec / 1000 > (s64)0x7fffffffffffffffULL /(s64)HZ)
		timeout_jiffies = -1;
	else
#endif
		timeout_jiffies = msecs_to_jiffies(timeout_msecs);
	} else {
		/ infinite (<0) or no (0) timeout
		timeout_jiffies = timeout_msecs;
	}
	
	return do_sys_poll(ufds, nfds, &timeout_jiffies);
}
```
它对超时参数稍作处理后，直接调用do_sys_poll。do_sys_poll 函数也位于位于`fs/select.c` 文件中，我们忽略其他代码：

```c
int do_sys_poll(struct pollfd __user *ufds, unsigned int nfds, s64 *timeout)
{
	...
	poll_intwait(&table);
	...
	fdcount = do_poll(nfds, head, &table, timeout);
	...
}
```
poll_initwait 函数非常简单，它初始化一个poll_wqueues 变量table：`poll_initwait > init_poll_funcptr(&pwq->pt, __pollwait); > pt->qproc = qproc;`即`table->pt->qproc = __pollwait，__pollwait `将在驱动的poll 函数里用到。
table 中的qproc 函数指针指向“__pollwait”。

```c
void poll_initwait(struct poll_wqueues *pwq)
{
	init_poll_funptr(&pwq->pt, __pollwait);
	pwd->error = 0;
	pwq->table = NULL;
	pwq->inline_index = 0;
}

static void __pollwait(struct file *filp, wait_queue_head_t *wait_address,
	poll_table *p)
{
	struct poll_table_entry *entry = poll_get_entry(p);
	if(!entry)
		return;
	
	get_file(filp);
	entry->filp = filp;
	entry->wait_address = wait_address;
	init_waitqueue_entry(&entry->wait, current);
	add_wait_queue(wait_address, &entry->wait);
}

static inline void init_poll_funcptr(poll_table_t *pt, poll_queue_proc qproc)
{
	pt->qproc = qproc;
}
```

do_poll 函数位于`fs/select.c`文件中，代码如下

```c
static int do_poll(unsigned int nfds, struct poll_list *list,
		struct poll_wqueues *wait, s64 *timeout)
{
	...
	for (;;) { //一个死循环
	...
		if(do_pollfd(pfd, pt)) {
			count++;
			pt = NULL;
		}

		if(count || !*timeout || signal_pending(current))
			break; //超时跳出，count 不为0时，就跳出返回到应用程序中去了
		
		count = wait->error;
		if(count)
			break;

		if(*timeout < 0) {
			/*wait indefinately */
			__timeout = MAX_SCHEDULE_TIMEOUT;
		} else if(unlikely(*timeout >= (s64)MAX_SCHEDULE_TIMEOUT -1) {
			/*
			 * wait for longer than MAX_SCHEDULE_TIMEOUT Do it in
			 * a loop
			 */
			__timeout = MAX_SCHEDULE_TIMEOUT - 1;
			*timeout = __timeout;
		} else {
			__timeout = *timeout;
			*timeout = 0;
		}

		__timeout = schedule_timeout(__timeout); //休眠
		if(*timeout >= 0)
			*timeout += __timeout;
	}
	
	__set_current_state(TASK_RUNNING);
		
	return count;
}
```
分析其中的代码，可以发现，它的作用如下：

* 循环，它退出的条件为：count非0，超时、有信号等待处理
	* count 非0 表示04 行的do_pollfd 至少有一个成功。
	* 11、12 行：发生错误

重点在do_pollfd 函数，后面再分析,`__timeout = schedule_timeout(__timeout);`让本进程休眠一段时间，注意：应用程序执行poll 调用后，如果①②的条件不满足，进程就会进入休眠。那么，谁唤醒呢？除了休眠到指定时间被**系统唤醒外**，还可以被**驱动程序**唤醒──记住这点，这就是为什么驱动的poll 里要调用poll_wait 的原因，后面分析。

```
app: poll 应用程序调用Poll时，内核就调用了sys_poll
kernel: sys_poll
			do_sys_poll(..., timeout_jiffies)
				poll_initwait(&table);//初始化
					init_poll_funptr;
				do_poll(nfds, head, &table, timeout);
					for(;;) //死循环
					{
						if(do_pollfd(pfd, pt) {
							mask = file->f_op->poll(file, pwait);
							count ++; //如果驱动的poll返回非0值，那么count ++;
							pt = NULL;
						}
						// break条件,count非0, 超时，有信号在等待处理
						if(count || !*timeout || signal_pending(current)
							break;
						
						//若上面条件都不满足则执行下面进行休眠，休眠时间为__timeout
						__timeout = schedule_timeout(__timeout);
					}
					// 当休眠到__timeout时间后，__timeout 变为0,就又开始for执行
					//而这时break中超时或者timeout为真，则执行了break，跳出了do_poll
```
do_pollfd 函数位于`fs/select.c`文件中，代码如下：

```c
static inline unsigned int do_pollfd(struct *pollfd, poll_table *pwait)
{
	...
	if(file->f_op && file->f_op->poll)
		mask = file->f_op->poll(file, pwait);
	...
}
```
可见，do_pollfd 函数就是调用我们的驱动程序里注册的poll 函数。这里有个“f_op”这就是
一个 file_operation。

```c
static struct file_operaiton second_drv_fops = {
	.owner = THIS_MODULE,
	.open = forth_drv_open,
	.read = forth_drv_read,
	.release = forth_drv_close,
	.poll = forth_drv_poll, //驱动程序中的poll
};

static unsigned forth_drv_poll(struct file *file, poll_table *wait)
{
	unsigned int mask = 0;
	poll_wait(file, &button_waitq, wait); //不会立即休眠
	if(ev_press)
		mask |= POLLIN | POLLRDNORM;

	return mask;
}
```


## 二、驱动程序
驱动程序里与poll 相关的地方有两处：一是构造file_operation 结构时，要定义自己的poll
函数。二是通过poll_wait 来调用上面说到的__pollwait 函数，pollwait 的代码如下：

```c
static inline void poll_wait(struct file *filp, wait_queue_head_t *wait_address, poll_table *p)
{
	if(p && wait_address) {
		p->qproc(filp, wait_address, p);
	}
}
```
`p->qproc`就是“__pollwait 函数”：在“do_sys_poll”函数中定义一个“poll_wqueues”结构的变量“table”，且在“do_sys_poll”中一个函数“`poll_initwait(&table);`”使用了这个变量。而"`void poll_initwait(struct poll_wqueues *pwq)`"中调用 了一个函数“init_poll_funcptr(&pwq->pt, __pollwait);”则，最后要看如下操作：“`init_poll_funcptr(&table->pt, __pollwait);`”，接着再看“`init_poll_funcptr()`”函数。

执行到驱动程序的poll_wait 函数时，进程并没有休眠，我们的驱动程序里实现的poll 函数
是不会引起休眠的。让进程进入休眠，是前面分析的do_sys_poll 函数的30 行“`__timeout = schedule_timeout(__timeout)`”。
poll_wait 只是把本进程挂入某个队列，应用程序调用poll > sys_poll > do_sys_poll >
poll_initwait，do_poll > do_pollfd > 我们自己写的poll 函数后，再调用schedule_timeout 进入休眠。如果我们的驱动程序发现情况就绪，可以把这个队列上挂着的进程唤醒。可见，poll_wait 的作用，只是为了让驱动程序能找到要**唤醒的进程**。

即使不用poll_wait，我们的程序也有机会被唤醒：`schedule_timeout(__timeout)`，只是要休眠__time_out 这段时间。

```c
static unsigned forth_drv_poll(struct file *file, poll_table *wait)
{
	unsigned int mask = 0;
	poll_wait(file, &button_waitq, wait); //不会立即休眠,只是让进程挂到队列里去
	//休眠时do_poll 中的 scedule_t timeout
	if(ev_press) //当前有数据可以返回到应用程序中,就返回mask |= POLLIN | POLLRDNORM
				//否则，mask = 0
		mask |= POLLIN | POLLRDNORM;

	// 若mask = 0， 则do_poll中的count ++ 就不会执行，则判断fi(count || !*timeout || signal_pending(current))，此count为0，则不会执行if下的break跳出，而是执行if后面的休眠 schedule_timeout()
	return mask;
}
```

# 现在来总结一下poll 机制
1. poll > sys_poll > do_sys_poll > poll_initwait，poll_initwait 函数注册一下回调函数__pollwait，它就是我们的驱动程序执行poll_wait 时，真正被调用的函数。


2.接下来执行file->f_op->poll，即我们驱动程序里自己实现的poll 函数
它会调用poll_wait 把自己挂入某个队列，这个队列也是我们的驱动自己定义的；
它还判断一下设备是否就绪。

3.如果设备未就绪，do_sys_poll 里会让进程休眠一定时间

4.进程被唤醒的条件有2：一是上面说的“一定时间”到了，二是被驱动程序唤醒。驱动程
序发现条件就绪时，就把“某个队列”上挂着的进程唤醒，这个队列，就是前面通过poll_wait
把本进程挂过去的队列。

5.如果驱动程序没有去唤醒进程，那么chedule_timeout(__timeou)超时后，会重复2、3 动
作，直到应用程序的poll 调用传入的时间到达。