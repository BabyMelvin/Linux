# Linux物理内存的组织形式
Linux把物理内存划分为三个层次管理：

* 存储节点(Node):CPU被划分为多个节点(node), 内存则被分簇, 每个CPU对应一个本地物理内存, 即一个CPU-node对应一个内存簇bank，即**每个内存簇被认为是一个节点**.
* 管理区(Zone):每个物理内存节点node被划分为多个内存管理区域, 用于表示不同范围的内存, 内核可以使用不同的映射方式映射物理内存
* 页面(Page):内存被细分为多个页面帧, 页面是最基本的页面分配的单位.

为了支持NUMA模型，也即CPU对不同内存单元的访问时间可能不同，此时系统的物理内存被划分为几个节点(node), 一个node对应一个内存簇bank，即每个内存簇被认为是一个节点

* 首先, 内存被划分为结点. 每个节点关联到系统中的一个处理器, 内核中表示为`pg_data_t`的实例. 系统中每个节点被链接到一个以NULL结尾的`pgdat_list`链表中<而其中的每个节点利用pg_data_tnode_next字段链接到下一节．而对于PC这种UMA结构的机器来说, 只使用了**一个**成为contig_page_data的静态pg_data_t结构.
* 接着各个节点又被划分为内存管理区域, 一个管理区域通过`struct zone_struct`描述, 其被定义为zone_t, 用以表示内存的某个范围, **低端范围的16MB被描述为ZONE_DMA**, 某些工业标准体系结构中的(ISA)设备需要用到它, 然后是可直接映射到内核的**普通内存域**`ZONE_NORMAL`,最后是超出了内核段的物理地址域ZONE_HIGHMEM, 被称为**高端内存**.　是系统中预留的可用内存空间, 不能被内核直接映射.
* 最后页帧(page frame)代表了系统内存的最小单位, 堆内存中的每个页都会创建一个struct page的一个实例. 传统上，把内存视为连续的字节，即内存为字节数组，内存单元的编号(地址)可作为字节数组的索引. 分页管理时，将若干字节视为一页，比如4K byte. 此时，内存变成了连续的页，即内存为页数组，每一页物理内存叫页帧，以页为单位对内存进行编号，该编号可作为页数组的索引，又称为页帧号.

在一个单独的节点内，任一给定CPU访问页面所需的时间都是相同的。然而，对不同的CPU，这个时间可能就不同。对每个CPU而言，内核都试图把耗时节点的访问次数减到最少这就要小心地选择CPU最常引用的内核数据结构的存放位置.

## 1.1 内存节点node
CPU被划分为多个节点(node), 内存则被分簇, 每个CPU对应一个本地物理内存, 即一个CPU-node对应一个内存簇bank，即每个内存簇被认为是一个节点系统的物理内存被划分为几个节点(node), 一个node对应一个内存簇bank，即每个内存簇被认为是一个节点.

在LINUX中引入一个数据结构`struct pglist_data `，来描述一个node，定义在`include/linux/mmzone.h`文件中。（这个结构被typedef pg_data_t）。

* 对于NUMA系统来讲， 整个系统的内存由一个node_data的pg_data_t指针数组来管理,
* 对于PC这样的UMA系统，使用`struct pglist_data contig_page_data` ，作为系统唯一的node管理所有的内存区域。（UMA系统中中只有一个node）

可以使用NODE_DATA(node_id)来查找系统中编号为node_id的结点, 参见NODE_DATA的定义
UMA结构下由于只有一个结点, 因此该宏总是返回全局的contig_page_data, 而与参数node_id无关.

```c
extern struct pglist_data *node_data[];
#define NODE_DATA(nid) (node_data[nid])
```
在UMA结构的机器中, 只有一个node结点即contig_page_data, 此时NODE_DATA直接指向了全局的contig_page_data, 而与node的编号nid无关,其中全局唯一的内存node结点contig_page_data定义在

```c
#ifndef CONFIG_NEED_MULTIPLE_NODES
extern struct pglist_data config_page_data;
#define NODE_DATA(nid) (&config_page_data)
#define NODE_MEM_MAP(nid) mem_map
#else
#endif
```
在分配一个页面时, Linux采用节点局部分配的策略, 从最靠近运行中的CPU的节点分配内存, 由于进程往往是在同一个CPU上运行, 因此从当前节点得到的内存很可能被用到

## 1.2 物理内存区域zone
因为实际的计算机体系结构有硬件的诸多限制, 这限制了页框可以使用的方式. 尤其是, Linux内核必须处理80x86体系结构的两种硬件约束.

* ISA总线的直接内存存储DMA处理器有一个严格的限制 : 他们只能对RAM的**前16MB进行寻址**
* 在具有大容量RAM的现代32位计算机中, CPU不能直接访问所有的物理地址, 因为线性地址空间太小, 内核不可能直接映射所有物理内存到线性地址空间, 我们会在后面典型架构(x86)上内存区域划分详细讲解x86_32上的内存区域划分 

因此Linux内核对不同区域的内存需要采用不同的管理方式和映射方式,为了解决这些制约条件，Linux使用了三种区:

* 1.ZONE_DMA : 这个区包含的页用来执行DMA操作。
* 2.ZONE_NOMAL : 这个区包含的都是能正常映射的页。
* 3.ZONE_HIGHEM : 这个区包”高端内存”，其中的页能不永久地映射到内核地址空间

而为了兼容一些设备的热插拔支持以及内存碎片化的处理, 内核也引入一些逻辑上的内存区.

* 1.ZONE_MOVABLE : 内核定义了一个伪内存域ZONE_MOVABLE, 在防止物理内存碎片的机制memory migration中需要使用该内存域. 供防止物理内存碎片的极致使用
* 2.ZONE_DEVICE : 为支持热插拔设备而分配的Non Volatile Memory非易失性内存

内核将每个簇所对应的node又被分成的称为管理区(zone)的块，它们各自描述在内存中的范围。一个管理区(zone)由struct zone结构体来描述，在linux-2.4.37之前的内核中是用typedef struct zone_struct zone_t数据结构来描述）

对于x86_32的机器，管理区(内存区域)类型如下分布

* 1.ZONE_DMA:`0~15MB`
* 2.ZONE_NORMAL:`16MB~895MB`
* 3.ZONE_HIGHMEM:896MB~物理内存结束

内核在初始化内存管理区时, 首先建立管理区表zone_table

```c
/*
* the zone_table array is used to look up the address of the struct zone corresponding to 
* a given zone number(ZONE_DMA, ZONE_NORMAL,or ZONE_HIGHMEM)
*/
zone_t *zone_table[MAX_NR_ZONES * MAX_NR_NODES];
EXPORT_SYMBOL(zone_table);
```
该表处理起来就像一个多维数组,

* MAX_NR_ZONES是一个节点中所能包容纳的管理区的最大数, 如3个, 定义在与zone区域的类型(ZONE_DMA, ZONE_NORMAL, ZONE_HIGHMEM)定义在一起. 当然这时候我们这些标识都是通过宏的方式来实现的, 而不是如今的枚举类型
* MAX_NR_NODES是可以存在的节点的最大数
* **函数EXPORT_SYMBOL**使得内核的变量或者函数可以被载入的**模块**(比如我们的驱动模块)所访问.

## 2.5 内存页page
大多数内核（kernel）的操作只使用`ZONE_NORMAL区域`，系统内存由很多固定大小的内存块组成的，这样的内存块称作为“页”（PAGE），

x86体系结构中，page的大小为4096个字节。

每个物理的页由一个struct page的数据结构对象来描述。页的数据结构对象都保存在mem_map全局数组中，该数组通常被存放在ZONE_NORMAL的首部，或者就在小内存系统中为装入内核映像而预留的区域之后。从载入内核的低地址内存区域的后面内存区域，也就是ZONE_NORMAL开始的地方的内存的页的数据结构对象，都保存在这个全局数组中。

## 2.6 高端内存
由于能够被Linux内核直接访问的ZONE_NORMAL区域的内存空间也是有限的，所以LINUX提出了高端内存（High memory）的概念，并且允许对高端内存的访问