工作队列一般用来做滞后的工作，比如在中断里面要做很多事，但是比较耗时，这时就可以把耗时的工作放到工作队列。说白了**就是系统延时调度的一个自定义函数**。

* 1.定义`struct work_struct irq_queue;`
* 2.初始化`INIT_WORK(&irq_queue,do_irq_queuework);`
* 3.调用方法：`schedule_work(&rq_queue);`

**注**:调用完毕后系统会释放此函数，所以如果想再次执行的话，就再次调用`schedule_work()`即可。另外，内核必须挂载文件系统才可以使用工作队列。我的理解是：工作队列也属于调度，如果内核挂了，他就不调度了，当然就不能用工作队列了。

Linux2.6内核使用了不少工作队列来处理任务，他在使用上和**tasklet最大**的不同是工作队列的函数可以使用休眠，而tasklet的函数是不允许使用休眠的。

工作队列的使用又分两种情况:

* 一种是利用`系统共享的工作队列`来添加自己的工作，这种情况处理函数不能消耗太多时间,这样会影响共享队列中其他任务的处理;
* 另外一种是创建自己的工作队列并添加工作。

# (一)利用系统共享的工作队列添加工作：

* 第一步：声明或编写一个工作处理函数

```c
void my_func();
```

* 第二步：创建一个工作结构体变量，并将处理函数和参数的入口地址赋给这个工作结构体变量

```c
//编译时创建名为my_work的结构体变量
//并把 函数入口地址 和 参数地址 赋给它;
DECLARE_WORK(my_work,my_func,&data); 
```
如果不想要在**编译时就用**`DECLARE_WORK()`创建并初始化工作结构体变量，也可以在**程序运行时**再用`INIT_WORK()`创建

```c
//创建一个名为my_work的结构体变量，创建后才能使用INIT_WORK()
struct work_struct my_work; 

//初始化已经创建的my_work，其实就是往这个结构体变量中添加处理函数的入口地址和data的地址
// 通常在驱动的open函数中完成
INIT_WORK(&my_work,my_func,&data);
```

* 第三步：将工作结构体变量添加入系统的共享工作队列

```c
schedule_work(&my_work); //添加入队列的工作完成后会自动从队列中删除
```

# (二)创建自己的工作队列来添加工作

* 第一步：声明工作处理函数和一个指向工作队列的指针

```c
void my_func();
struct workqueue_struct *p_queue;
```

* 第二步：创建自己的工作队列和工作结构体变量(通常在open函数中完成)

```c
//创建一个名为my_queue的工作队列并把工作队列的入口地址赋给声明的指针
p_queue=create_workqueue("my_queue"); 

//创建一个工作结构体变量并初始化，和第一种情况的方法一样
struct work_struct my_work;
INIT_WORK(&my_work, my_func, &data); 
```

* 第三步：将工作添加入自己创建的工作队列等待执行

```c
//作用与schedule_work()类似
//不同的是将工作添加入p_queue指针指向的工作队列而不是系统共享的工作队列
queue_work(p_queue, &my_work);
```

* 第四步：删除自己的工作队列

```
destroy_workqueue(p_queue); //一般是在close函数中删除
```