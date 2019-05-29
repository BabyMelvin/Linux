# Linux内核启动分析
`start_kernel`在 `/linux/init/main.c`中定义：这个函数是内核由引导程序引导以后，由自解压程序解压以后执行的第一个函数，可以认为是整个内核的入口函数，以后我分析的代码全部从这个函数开始！

这个函数做的事情相对比较简单，就是线性的初始化一些内核的基础机制，如中断，内存管理，进程管理，信号，文件系统，KO等！最后就启动一个init线程，init线程再读取文件系统里的`init程序`，做为系统的第一个进程而存在！

其实，start_kernel函数是0是做为0号进程存在的，它在最后就是空转CPU：

```c
asmlinkage __visiable void __init start_kernel(void)
{
	char *command_line;
	char *after_dashes;

	//①need to run as early as possible,to initialize the lockdep hash
	lockdep_init();
	set_task_stack_end_magic(&init_task);
	smp_setup_processor_id();
	debug_object_early_init();

	//②set up the the initial canary(金丝雀) ASAP(as soon as possible)
	boot_init_stack_canary();
	cgroup_init_early();
	local_irq_disable();
	early_boot_irqs_disable = true;

	//③interrupts ars still disable. do neccessay setups,then enable them
	boot_cpu_init();
	page_address_init();
	pr_notice("%s", linux_banner);
	setup_arch(&command_line);
	mm_init_cpumask(&init_mm);
	setup_command_line(command_line);
	setup_nr_cpu_ids();
	setup_per_cpu_areas();
	smp_prepare_boot_cpu();//arch-specific boot-cpu hooks
	
	build_all_zonelists(NULL, NULL);
	page_alloc_init();
	
	pr_notice("Kernel command line:%s\n", boot_command_line);
	parse_early_param();
	after_dashes = parse_args("Booting kernel",static_command_line, __start__param, __stop__param - 
		__start__param, -1, -1, &unknown_bootoption);	
	if(!IS_ERR_OR_NULL(after_dashes)) {
		parse_args("Setting init args", after_dashes, NULL, 0, -1, -1, set_init_arg);
	}
	jump_label_init();

	//④these use large bootmem allocations and must precede kmem_cache_init()
	setup_log_buf(0);
	pidhash_init();
	vfs_caches_init_early();
	sort_main_extable();
	trap_init();
	mm_init();

	//⑤set up the scheduler prior starting any interrupts(sunch as the timer interrupt).Full topology setup happends
	// at smp_init() time - but meanwhile we still have a funcitoning scheduler
	sched_init();
	
	//⑥disable preemption(先占，先买权) - early bootup scheduling is extremely fragile(脆弱的) until we cpu_idle() for
	//⑦ the first time
	preempt_disable();
	if(WARN(!irqs_disable(), "Interrupts were enabled very early,fixing it\n"))
		local_irq_disable();
	idr_init_cache();
	rcu_init();
	
	//⑧trace_printk() and trace points may be used after this
	trace_init();
	 
	context_tracking_init();
	radix_tree_init();
	//⑨init some links before init_ISA_irqs();ISA:工业标准结构
	early_irq_init();
	init_IRQ();
	tick_init();
	rcu_init_nohz();
	init_timers();
	hrtimers_init();
	softirq_init();
	timekeeping_init();
	time_init();
	sched_clock_postinit();
	perf_event_init();
	profile_init();
	call_funtion_init();
	WARN(!irq_disable(),"Interrupts were enabled early!");
	early_boot_irqs_disable = false;
	local_irq_enable();
	
	kmem_cache_init_late();

	//⑩HACK ALERT! this is early.We're enabling the console before we've done PCI setups etc,and console_init()
	//must be aware of this.But we do want output earyly, in case something goes wrong
	console_init();
	if(panic_later)
		panic("Too many boot %s vars at %s", panic_later, panic_param);
	lockdep_info();

	//十一need to run this when irqs are enabled, because it wants to self-test [hard/soft]-irqs on/off inversion bugs too
	locking_selftest();

	page_ext_init();
	debug_objects_mem_init();
	kmemleak_init();
	setup_per_cpu_pageset();
	numa_policy_init();
	if(late_time_init)
		late_time_init();
	sched_clock_init();
	calibrate_delay();
	pidmap_init();
	anon_vma_init();
	acpi_early_init();
	
	thread_info_cache_init();
	cred_init();
	fork_init(totalram_pages);
	proc_caches_init();
	buffer_init();
	key_init();
	security_init();
	dbg_late_init();
	vfs_caches_init(totalram_pages);
	signals_init();

	//十二rootfs populating might need page-writeback
	page_writeback_init();
	proc_root_init();
	nsfs_init();
	cgroup_init();
	cpuset_init();
	taskstats_init_early();
	delayacct_init();
	
	check_bugs();
	acpi_subsystem_init();
	sfi_init_late();
	
	if(efi_enable(EFI_RUNTIME_SERVICES)) {
		efi_late_init();
		efi_free_boot_services();
	}
	ftrace_init();
	//do the reset non-__init'ed, we're now alive
	reset_init();
}
```

# 具体分析

## 1
`char *command_line;`用来存放bootloader传递过来的参数

* `lookdep_init()`

建立一个哈希表(hash tables)，就是一个前后指向的指针结构体数组.函数的主要作用是初始化锁的状态跟踪模块。由于内核大量使用锁来进行多进程多处理器的同步操作，死锁就会在代码不合理的时候出现，但是要定位哪个锁比较困难，用哈希表可以跟踪锁的使用状态。

* classhash_table:数组长度`1<< 12`
* chainhash_table:数组长度`1<<12`
死锁情况：

* 一个进程递归加锁同一把锁；
* 同一把锁在两次中断中加锁；
* 几把锁形成闭环死锁

* `set_task_stack_end_magic(&init_task); `
其中init_task，是在`init/init_task.c`中EXPORT_SYMBOL(init_task);init_task即手工创建的PCB，0号进程即最终的idle进程。STACK_END_MAGIC ，作为溢出检测

```
struct task_struct init_stask = INIT_TASK(init_task);
EXPORT_SYMBOL(init_task);

//init_task.h
//INIT_TASK is used to setup the first task table, touch at your own risk!Base = 0,limit = 0x1fffff(=2MB)

#define INIT_TASK(tsk) \
{   
	.state = 0, \        
	.stack = &init_thread_info, \
	....
```

* `smp_setup_processor_id();`

针对SMP处理器，用于获取当前CPU的硬件ID，如果不是多核，函数为空 (判断是否定义了CONFIG_SMP，如果定义了调用read_cpuid_mpidr读取寄存器CPUID_MPIDR的值，就是当前正在执行初始化的CPU ID，为了在初始化时做个区分，初始化完成后，所有处理器都是平等的，没有主从

* `debug_objects_early_init();`
初始化哈希桶(hash buckets)并将static object和pool object放入poll列表，这样堆栈就可以完全操作了 （这个函数的主要作用就是对调试对象进行早期的初始化，就是HASH锁和静态对象池进行初始化，执行完后，object tracker已经开始完全运作了

## 2.

* `boot_init_stack_canary();`初始化堆栈保护的值，防止栈溢出
* `cgroup_init_early();`:在系统启动时初始化cgroups，同时初始化需要early_init的子系统 （这个函数作用是控制组(control groups)早期的初始化，控制组就是定义一组进程具有相同资源的占有程度，比如，可以指定一组进程使用CPU为30%，磁盘IO为40%，网络带宽为50%。目的就是为了把所有进程分配不同的资源.
* `local_irq_disable();`:关闭当前CPU的所有中断响应，操作CPSR寄存器。对应后面的

## 3.

* `boot_cpu_init();`:设置当前引导系统的CPU在物理上存在，在逻辑上可以使用，并且初始化准备好，即激活当前CPU （在多CPU的系统里，内核需要管理多个CPU，那么就需要知道系统有多少个CPU，在内核里使用cpu_present_map位图表达有多少个CPU，每一位表示一个CPU的存在。如果是单个CPU，就是第0位设置为1。虽然系统里有多个CPU存在，但是每个CPU不一定可以使用，或者没有初始化，在内核使用cpu_online_map位图来表示那些CPU可以运行内核代码和接受中断处理。随着移动系统的节能需求，需要对CPU进行节能处理，比如有多个CPU运行时可以提高性能，但花费太多电能，导致电池不耐用，需要减少运行的CPU个数，或者只需要一个CPU运行。这样内核又引入了一个cpu_possible_map位图，表示最多可以使用多少个CPU。在本函数里就是依次设置这三个位图的标志，让引导的CPU物理上存在，已经初始化好，最少需要运行的CPU。
* `page_address_init();`初始化高端内存的映射表 （在这里引入了高端内存的概念，那么什么叫做高端内存呢？为什么要使用高端内存呢？其实高端内存是相对于低端内存而存在的，那么先要理解一下低端内存了。在32位的系统里，最多能访问的总内存是4G，其中3G空间给应用程序，而内核只占用1G的空间。因此，内核能映射的内存空间，只有1G大小，但实际上比这个还要小一些，大概是896M，另外128M空间是用来映射高端内存使用的。因此0到896M的内存空间，就叫做低端内存，而高于896M的内存，就叫高端内存了。如果系统是64位系统，当然就没未必要有高端内存存在了，因为64位有足够多的地址空间给内核使用，访问的内存可以达到10G都没有问题。在32位系统里，内核为了访问超过1G的物理内存空间，需要使用高端内存映射表。比如当内核需要读取1G的缓存数据时，就需要分配高端内存来使用，这样才可以管理起来。使用高端内存之后，32位的系统也可以访问达到64G内存。在移动操作系统里，目前还没有这个必要，最多才1G多内存
* `pr_notice("%s", linux_banner);`:输出各种信息(Linux_banner是在`kernel/init/version.c`中定义的，这个字符串是编译脚本自动生成的
* `setup_arch(&command_line);`内核架构相关初始化函数，是非常重要的一个初始化步骤。其中包含了处理器相关参数的初始化、内核启动参数(tagged list)的获取和前期处理、内存子系统的早期初始化(bootmem分配器
* `mm_init_cpumask(&init_mm);`:每一个任务都有一个mm_struct结构来管理内存空间，init_mm是内核的mm_struct
* `setup_command_line(command_line);`:对cmdline进行备份和保存
* `setup_nr_cpu_ids();`设置最多有多少个nr_cpu_ids结构
* `setup_per_cpu_areas();`:*为系统中每个CPU的per_cpu变量申请空间，同时拷贝初始化段里数据（`.data.percpu`）
* `smp_prepare_boot_cpu();`:为SMP系统里引导CPU(boot-cpu)进行准备工作。在ARM系统单核里是空函数
* `build_all_zonelists(NULL, NULL);`:设置内存管理相关的node（节点，每个CPU一个内存节点）和其中的zone（内存域，包含于节点中，如）数据结构,以完成内存管理子系统的初始化，并设置bootmem分配器
* `page_alloc_init();`:设置内存页分配通知器
* `pr_notice("Kernel command line: %s\n", boot_command_line);`
* `parse_early_param();`:解析cmdline中的启动参数
* `after_dashes = parse_args`:这行代码主要对传入内核参数进行解释，如果不能识别的命令就调用最后参数的函数
* `jump_label_init();`:

## 4.

* `setup_log_buf(0);`使用bootmeme分配一个记录启动信息的缓冲区
* `pidhash_init();`:进程ID的HASH表初始化，用bootmem分配并初始化PID散列表，由PID分配器管理空闲和已指派的PID，这样可以提供通PID进行高效访问进程结构的信息。LINUX里共有四种类型的PID，因此就有四种HASH表相对应。
* `vfs_caches_init_early();`:前期虚拟文件系统(vfs)的缓存初始化
* `sort_main_extable();`:对内核异常表(exception table)按照异常向量号大小进行排序，以便加速访问
* `trap_init();`对内核陷阱异常进行初始化，在ARM系统里是空函数，没有任何的初始化
* `mm_init();`标记哪些内存可以使用，并且告诉系统有多少内存可以使用，当然是除了内核使用的内存以外
	* 初始化内核内存分配器，包括六个子函数 
	* 1、`page_cgroup_init_flatmem()；`获取page_cgroup所需内存 
	* 2、`mem_init；`关闭并释放bootmem分配器，打印内存信息，内核启动时看到Virtual kernel memory layout:的信息就是这个函数的 
	* 3、`kmem_cache_init()；`初始化slab分配器 
	* 4、`percpu_init_late()；`PerCPU变量系统后期初始化 
	* 5、`pgtable_cache_init()；`也表缓存初始化，arm中是个空函数 
	* 6、`vmalloc_init()；`初始化虚拟内存分配器

## 5.

* `sched_init();`:对进程调度器的数据结构进行初始化，创建运行队列，设置当前任务的空线程，当前任务的调度策略为CFS调度器

## 6,7.

* `preempt_disable();`:关闭优先级调度。由于每个进程任务都有优先级，目前系统还没有完全初始化，还不能打开优先级调度。
* `if (WARN(!irqs_disabled(),"Interrupts were enabled *very* early, fixing it\n")) `:这段代码主要判断是否过早打开中断，如果是这样，就会提示，并把中断关闭
* `idr_init_cache();`:为IDR机制分配缓存，主要是为 structidr_layer结构体分配空间
* `rcu_init();`:初始化直接读拷贝更新的锁机制。 Read-Copy Update （RCU主要提供在读取数据机会比较多，但更新比较的少的场合，这样减少读取数据锁的性能低下的问题

## 8.

* `context_tracking_init();`:
* `radix_tree_init();`:内核radis 树算法初始化

## 9.

* `early_irq_init();`:前期外部中断描述符初始化，主要初始化数据结构
* `init_IRQ();`:对应架构特定的中断初始化函数，在ARM中就是`machine_desc->init_irq()`，就是运行设备描述结构体中的init_irq函数
* `tick_init();`:初始化内核时钟系统，tick control，调用clockevents_register_notifier，就是监听时钟变化事件 （这个函数主要作用是初始化时钟事件管理器的回调函数，比如当时钟设备添加时处理。在内核里定义了时钟事件管理器，主要用来管理所有需要周期性地执行任务的设备
* `rcu_init_nohz();`
* `init_timers();`:初始化引导CPU的时钟相关的数据结构，注册时钟的回调函数，当时钟到达时可以回调时钟处理函数，最后初始化时钟软件中断处理
* `hrtimers_init();`:初始化高精度的定时器，并设置回调函数.
* `softirq_init();`:初始化软件中断，软件中断与硬件中断区别就是中断发生时，软件中断是使用线程来监视中断信号，而硬件中断是使用CPU硬件来监视中断。
* `timekeeping_init();`:初始化系统时钟计时，并且初始化内核里与时钟计时相关的变量.
* `time_init();`:初始化系统时钟。开启一个硬件定时器，开始产生系统时钟就是system_timer的初始化,`arch/arm/mach-msm/board-*.c` 
* `sched_clock_postinit();`
* `perf_event_init();`:CPU性能监视机制初始化，此机制包括CPU同一时间执行指令数，cache miss数，分支预测失败次数等性能参数
* `profile_init();`:分配内核性能统计保存的内存，以便统计的性能变量可以保存到这里
* `call_function_init();`:初始化所有CPU的call_single_queue，同时注册CPU热插拔通知函数到CPU通知链中
* `WARN(!irqs_disabled(), "Interrupts were enabled early\n");`   early_boot_irqs_disabled = false; 对应前面的local_irq_disable() (打开本CPU的中断，也即允许本CPU处理中断事件，在这里打开引CPU的中断处理。如果有多核心，别的CPU还没有打开中断处理。)
* `kmem_cache_init_late();`:这是内核内存缓存(slab分配器)的后期初始化，当初始化完成之后，就可以使用通用内存缓存了

## 10.

* `console_init();`:初始化控制台，从这个函数之后就可以输出内容到控制台了。在这个函数初化之前，都没有办法输出内容，就是输出，也是写到输出缓冲区里，缓存起来，等到这个函数调用之后，就立即输出内容
* `if (panic_later)  panic("Too many boot %s vars at %s", panic_later,panic_param);`:判断分析输入的参数是否出错，如果有出错，就启动控制台输出之后，立即打印出错的参数，以便用户立即看到出错的地方。
* `lockdep_info();`打印锁的依赖信息，用来调试锁。通过这个函数可以查看目前锁的状态，以便可以发现那些锁产生死锁，那些锁使用有问题。

## 11.
* `locking_selftest();`:测试锁的API是否使用正常，进行自我测试。比如测试自旋锁、读写锁、一般信号量和读写信号量。

```c
#ifdef CONFIG_BLK_DEV_INITRD
 if (initrd_start && !initrd_below_start_ok &&
     page_to_pfn(virt_to_page((void *)initrd_start)) < min_low_pfn) {
  pr_crit("initrd overwritten (0x%08lx < 0x%08lx) - disabling it.\n",
      page_to_pfn(virt_to_page((void *)initrd_start)),
      min_low_pfn);
  initrd_start = 0;
 }
#endif
```

检查initrd的位置是否符合要求，也就是判断传递进来initrd_start对应的物理地址是否正常，如果有误就打印错误信息，并清零initrd_start。

* `page_cgroup_init();`:初始化容器组的页面内存分配，mem_cgroup是cgroup体系中提供的用于memory隔离的功能，
* `debug_objects_mem_init();`:这个函数是创建调试对象内存分配初始化，所以紧跟内存缓存初始化后面
* `kmemleak_init();`:内核内存泄漏检测机制初始化；
* `setup_per_cpu_pageset();`:创建每个CPU的高速缓存集合数组并初始化，此前只有启动页组。因为每个CPU都不定时需要使用一些页面内存和释放页面内存，为了提高效率，就预先创建一些内存页面作为每个CPU的页面集合。
* `numa_policy_init();`:初始化NUMA的内存访问策略。所谓NUMA，它是NonUniform Memory AccessAchitecture(非一致性内存访问)的缩写，主要用来提高多个CPU访问内存的速度。因为多个CPU访问同一个节点的内存速度远远比访问多个节点的速度来得快。
* `if (late_time_init) late_time_init();`:主要运行时钟相关后期的初始化功能.
* `sched_clock_init();`:对每个CPU进行系统进程调度时钟初始化
* `calibrate_delay();`:主要计算CPU需要校准的时间，这里说的时间是CPU执行时间。如果是引导CPU，这个函数计算出来的校准时间是不需要使用的，主要使用在非引导CPU上，因为非引导CPU执行的频率不一样，导致时间计算不准确。BogoMIPS值，也是衡量cpu性能的标志
* `pidmap_init();`:进程位图初始化，一般情况下使用一页来表示所有进程占用情况。
* `anon_vma_init();`:初始化反向映射的匿名内存，提供反向查找内存的结构指针位置，快速地回收内存。
* `acpi_early_init();`:这个函数是初始化ACPI电源管理。高级配置及电源接口(Advanced Configuration and Power Interface)ACPI规范介绍ACPI能使软、硬件、操作系统（OS），主机板和外围设备，依照一定的方式管理用电情况，系统硬件产生的Hot-Plug事件，让操作系统从用户的角度上直接支配即插即用设备，不同于以往直接通过基于BIOS 的方式的管理。

```c
#ifdef CONFIG_X86
 if (efi_enabled(EFI_RUNTIME_SERVICES))
  efi_enter_virtual_mode();
#endif
```

初始化EFI的接口，并进入虚拟模式。EFI是ExtensibleFirmware Interface的缩写，就是INTEL公司新开发的BIOS接口。

* `thread_info_cache_init();`:线程信息(thread_info)的缓存初始化。
* `cred_init();`:任务信用系统初始化 
* `fork_init(totaltam_pages)`:根据当前物理内存计算出来可以创建进程（线程）的最大数量，并进行进程环境初始化，为task_struct分配空间。
* `proc_caches_init();`:进程缓存初始化，为进程初始化创建机制所需的其他数据结构申请空间
* `buffer_init();`:初始化文件系统的缓冲区，并计算最大可以使用的文件缓存
* `key_init();`:初始化内核安全键管理列表和结构，内核密钥管理系统
* `security_init();`:初始化内核安全管理框架，以便提供`访问文件／登录等权限`。
* `dbg_late_init();`:内核调试系统后期初始化
* `vfs_caches_init(totalram_pages);`:虚拟文件系统进行缓存初始化，提高虚拟文件系统的访问速度
* `signals_init();`:初始化信号队列缓存。信号管理系统
* `page_writeback_init();`:页面写机制初始化
* `proc_root_init();`:初始化系统进程文件系统，主要提供内核与用户进行交互的平台，方便用户实时查看进程的信息。
* `cgroup_init();`:进程控制组正式初始化，主要用来为进程和其子程提供性能控制。比如限定这组进程的CPU使用率为20％ 
* `cpuset_init();`:初始化CPUSET，CPUSET主要为控制组提供CPU和内存节点的管理的结构.
* `taskstats_init_early();`:任务状态早期初始化，为结构体获取高速缓存，并初始化互斥机制。任务状态主要向用户提供任务的状态信息。
* `delayacct_init();`:任务延迟机制初始化，初始化每个任务延时计数。当一个任务等CPU运行，或者等IO同步时，都需要计算等待时间.
* `check_bugs();`:检查CPU配置、FPU等是否非法使用不具备的功能，检查CPU BUG，软件规避BUG
* `sfi_init_late();`:SFI 初始程序晚期设置函数
* `ftrace_init();`:功能跟踪调试机制初始化，初始化内核跟踪模块，ftrace的作用是帮助开发人员了解Linux 内核的运行时行为，以便进行故障调试或性能分析 function trace.
* `rest_init();`:剩余的初始化，至此，内核已经开始工作了