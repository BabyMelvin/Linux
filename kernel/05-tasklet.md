对于内核中常用的中断处理机制做一些总结，方便在合适的时候采用合适的机制。

* `tasklet`和`work_queue`是延期执行工作的机制，实现基于软中断；
* `completion`的实现基于`wait_queue`。

# 1.tasklet
**小进程**，主要用于执行一些小任务，对这些任务使用全功能进程比较浪费。也称为`中断下半部`，在处理软中断时执行。

## 1.1 定义

```c
struct tasklet_struct{
	struct tasklet_struct* next;
	unsigned long state;
	atomic_t count;
	void (*func)(unsigned long);
	unsigned long data;
};
```

## 1.2 相关定义

```
#define DECLARE_TASKLET(name,func,data) \
	struct tasklet_struct name ={NULL,0,ATOMIC_INIT(0),func,data};

#define DECLARE_TASKLET_DISABLE(name,func,data) \
	struct tasklet_struct name = {NULL,0,ATOMIC_INIT(1),func,data};

enum{
	TASKLET_STATE_SCHED,//tasklet is scheduled for executation
	TASKLET_STATE_RUN   //tasklet is running(SMP only)
}；
```

## 1.3 初始化及销毁tasklet的方法

```c
void tasklet_init(struct tasklet_struct *t,
                  void (*func)(unsigned long), unsigned long data)
{
        t->next = NULL;
        t->state = 0;
        atomic_set(&t->count, 0);
        t->func = func;
        t->data = data;
}
 
void tasklet_kill(struct tasklet_struct *t)
{
        if (in_interrupt())
                printk("Attempt to kill tasklet from interrupt\n");
 
        while (test_and_set_bit(TASKLET_STATE_SCHED, &t->state)) {
                do {
                        yield();
                } while (test_bit(TASKLET_STATE_SCHED, &t->state));
        }
        tasklet_unlock_wait(t);
        clear_bit(TASKLET_STATE_SCHED, &t->state);
}
```

## 1.4 调度tasklet的方法

```c
static inline void tasklet_schedule(struct tasklet_struct *t)
{
        if (!test_and_set_bit(TASKLET_STATE_SCHED, &t->state))
                __tasklet_schedule(t);
}
 
void __tasklet_schedule(struct tasklet_struct *t)
{
        unsigned long flags;
 
        local_irq_save(flags);
        t->next = NULL;
        *__this_cpu_read(tasklet_vec.tail) = t;
        __this_cpu_write(tasklet_vec.tail, &(t->next));
        raise_softirq_irqoff(TASKLET_SOFTIRQ);
        local_irq_restore(flags);
}
```

## 1.5 使用方法

```c
tasklet_init->tasklet_schedule
```

# 2.wait_queue
用于使进程等待某一事件的发生，**无须频繁轮讯**，**进程在等待期间睡眠**，在事件发生时由内核自动唤醒。

## 2.1 方法一`add_wait_queue`和`wake_up`组合
当nand控制器被一个进程使用时，其余进程被放入等待队列；待第一个进程使用结束后，调用wake_up唤醒等待队列，下一个进程获得nand控制器的使用权。

### 2.1.1声明

```c

#define DECLARE_WAITQUEUE(name, tsk)                                    \
        wait_queue_t name = __WAITQUEUE_INITIALIZER(name, tsk)
 
 
#define __WAITQUEUE_INITIALIZER(name, tsk) {                            \
        .private        = tsk,                                          \
        .func           = default_wake_function,                        \
        .task_list      = { NULL, NULL } }
        
void add_wait_queue(wait_queue_head_t *q, wait_queue_t *wait)
{
        unsigned long flags;
 
        wait->flags &= ~WQ_FLAG_EXCLUSIVE;
        spin_lock_irqsave(&q->lock, flags);
        __add_wait_queue(q, wait);
        spin_unlock_irqrestore(&q->lock, flags);
}
```

### 2.1.2使用实例

```c
//nand.c
static init nane_test_get_device(struct mtd_info* mtd,int new_state){
	struct nand_chip* chip=mtd->priv;
	spinlock_t *lock=&chip->controller->lock;
	wait_queue_head_t *wq=&chip->controller->wq;
	DECLARE_WAITQUEUE(wait,current);
retry:
		spin_lock(lock);
        /* Hardware controller shared among independent devices */
        if (!chip->controller->active)
                chip->controller->active = chip;
 
        if (chip->controller->active == chip && chip->state == FL_READY) {
                chip->state = new_state;
                spin_unlock(lock);
                return 0;
        }    
        if (new_state == FL_PM_SUSPENDED) {
                if (chip->controller->active->state == FL_PM_SUSPENDED) {
                        chip->state = FL_PM_SUSPENDED;
                        spin_unlock(lock);
                        return 0;
                }    
        }    
        set_current_state(TASK_UNINTERRUPTIBLE);
        add_wait_queue(wq, &wait);
        spin_unlock(lock);
        schedule();
        remove_wait_queue(wq, &wait);
        goto retry;
}
static void nand_test_release_device(struct mtd_info *mtd)
{
        struct nand_chip *chip = mtd->priv;
 
        /* Release the controller and the chip */
        spin_lock(&chip->controller->lock);
        chip->controller->active = NULL;
        chip->state = FL_READY;
        wake_up(&chip->controller->wq);
        spin_unlock(&chip->controller->lock);
}
```

## 2.2`prepare_to_wait`和`wake_up`组合
可作为实现阻塞的方式：

```c
#define DEFINE_WAIT_FUNC(name, function)                                \
        wait_queue_t name = {                                           \
                .private        = current,                              \
                .func           = function,                             \
                .task_list      = LIST_HEAD_INIT((name).task_list),     \
        }
 
#define DEFINE_WAIT(name) DEFINE_WAIT_FUNC(name, autoremove_wake_function)
```
`autoremove_wake_function`会调用`default_wake_function`，然后将所属等待队列成员从等待队列中删除。

```c
void
prepare_to_wait(wait_queue_head_t *q, wait_queue_t *wait, int state)
{
        unsigned long flags;
 
        wait->flags &= ~WQ_FLAG_EXCLUSIVE;
        spin_lock_irqsave(&q->lock, flags);
        if (list_empty(&wait->task_list))
                __add_wait_queue(q, wait);
        set_current_state(state);
        spin_unlock_irqrestore(&q->lock, flags);
}
 
static long do_CustomEvent_wait(long eventNum)                                                
{
        struct CustomEvent *tmp = NULL;                                                       
        struct CustomEvent *prev = NULL;                                                      
 
        pr_info("Enten into %s, the eventNum is %d\n", __func__, eventNum);                   
        if((tmp = FindEventNum(eventNum, &prev)) != NULL) {                                   
                DEFINE_WAIT(wait);                                                            
                prepare_to_wait(tmp->p, &wait, TASK_INTERRUPTIBLE);                           
 
                schedule();                                                                   
                finish_wait(tmp->p, &wait);                                                   
                return eventNum;
        }
        return -1;
}
 
static long do_CustomEvent_signal(long eventNum)
{
        struct CustomEvent *tmp = NULL;
        struct CustomEvent *prev = NULL;
 
        pr_info("Enten into %s, the eventNum is %d\n", __func__, eventNum);
        if(!(tmp = FindEventNum(eventNum, &prev)))
                return 0;
 
        wake_up(tmp->p);
        return 1;
}
```

## 2.3 `wait_event` 和 `wake_up`组合
该方法首先将进程的状态设为 TASK_UNINTERRUPTIBLE， 之后判断条件是否满足；

* 条件不满足时，通过`schedule()`将该进程从runqueue队列中移除，该进程进入睡眠状态；只有当某进程调用wake_up才能再次唤醒该进程；进程被唤醒后，再次将该进程的状态置为TASK_UNINTERRUPTIBLE，然后判断条件是否满足。
* 条件满足时结束循环，由finish_wait将进程的状态设置为TASK_RUNNING，并从等待队列的链表中移除相应项。

```c
#define __wait_event(wq, condition)                                     \
do {                                                                    \
        DEFINE_WAIT(__wait);                                            \
                                                                        \
        for (;;) {                                                      \
                prepare_to_wait(&wq, &__wait, TASK_UNINTERRUPTIBLE);    \
                if (condition)                                          \
                        break;                                          \
                schedule();                                             \
        }                                                               \
        finish_wait(&wq, &__wait);                                      \
} while (0)
 
#define wait_event(wq, condition)                                       \
do {                                                                    \
        if (condition)                                                  \
                break;                                                  \
        __wait_event(wq, condition);                                    \
} while (0)
```

# 3.work_queue
由于在中断中不能进行阻塞型操作，而有时候需要在中断时读取某些内存单元或寄存器的值，此时可以考虑利用工作队列来实现。

## 3.1工作队列的定义

工作队列是将操作延期的一种手段。因为他们是通过守护进程在用户上下文执行，函数可以睡眠任意长的时间，这与内核无关。替换了之前的kevented机制。

## 3.2 工作队列的使用


使用工作队列，主要有以下三个步骤：

* 1实现工作任务处理函数
* 2创建并初始化工作任务
* 3将工作任务添加到某工作队列中，等待系统调度

```c
//kernel/workqueue.c
int schedule_work(struct work_struct *work)
 
int schedule_delayed_work(struct work_struct *dwork, unsigned long delay)
```

### 3.2.1.定义工作任务处理函数

```c
static void func(struct work_struct *work)
{
......
}
```

### 3.2.2.创建并初始化工作任务
可采用两种方式创建并初始化工作任务：
1) 先创建工作任务，后绑定处理函数：`struct work_struct xxx_wq;//创建工作任务`

在模块初始化的时候：`INIT_WORK(&xxx_wq,func);//初始化工作任务，工作任务需要执行的是函数func`

2) 创建工作任务的同时初始化:`DECLARE_WORK(xxx_wq, func); `

### 3.2.3  添加到工作队列，等待系统调度
 在中断处理函数中将工作任务添加到工作队列，等待系统调度;

```c
static inline bool queue_work(struct workqueue_struct *wq, struct work_struct *work)
{
        return queue_work_on(WORK_CPU_UNBOUND, wq, work);
}
```

用于将某工作任务添加到某工作队列

内核创建了一个标准的工作队列events，内核的各个部分若无必要创建独立的工作队列，可直接使用该工作队列。

```c
static inline bool schedule_work(struct work_struct *work)//将工作任务xxx_wq添加到标准工作队列events中，中断处理完成之后可以立即执行func
{
        return queue_work(system_wq, work);
}
```