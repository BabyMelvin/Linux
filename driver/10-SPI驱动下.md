# Linux SPI总线和设备驱动架构之三：SPI控制器驱动
整个SPI驱动架构可以分为`协议驱动`、`通用接口层`和`控制器驱动`三大部分。其中，控制器驱动负责最底层的数据收发工作，为了完成数据的收发工作，控制器驱动需要完成以下这些功能：

* 1.申请必要的硬件资源，例如中断，DMA通道，DMA内存缓冲区等等；
* 2. 配置SPI控制器的工作模式和参数，使之可以和相应的设备进行正确的数据交换工作；
* 3.向通用接口层提供接口，使得上层的协议驱动可以通过通用接口层访问控制器驱动；
* 4. 配合通用接口层，完成数据消息队列的排队和处理，直到消息队列变空为止；

# 定义控制器设备
SPI控制器遵循linux的设备模型框架，所以，**一个SPI控制器在代码中对应一个device结构**，对于嵌入式系统，我们通常把**SPI控制器**作为一个平台设备来对待，所以，对于我们来说，只要在板级的代码中为SPI控制器定义一个platform_device结构即可。下面以Samsung的SOC芯片：S3C6410，做为例子，看看如何定义这个platform_device。以下的代码来自：`/arch/arm/plat-samsung/devs.c`中：

```c
static struct resource s3c64xx_spi0_resource[] = {
	[0] = DEFINE_RES_MEM(S3C_PA_SPI0, SZ_256),
	[1] = DEFINE_RES_DMA(DMACH_SPI0_TX),
	[2] = DEFINE_RES_DMA(DMACH_SPI0_RX),
	[3] = DEFINE_RES_IRQ(IRQ_SPI0),
};
 
struct platform_device s3c64xx_device_spi0 = {
	.name		= "s3c6410-spi",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(s3c64xx_spi0_resource),
	.resource	= s3c64xx_spi0_resource,
	.dev = {
		.dma_mask		= &samsung_device_dma_mask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
};
```
由此可见，在这个platform_device中，我们定义了控制器所需的寄存器地址、DMA通道资源和IRQ编号，设备的名字定义为：`s3c64xx-spi`，这个名字用于后续和相应的控制器驱动相匹配。在machine的初始化代码中，我们需要注册这个代表SPI控制器的平台设备，另外，也会通过s3c64xx_spi0_set_platdata函数设置平台相关的参数供后续的控制器驱动使用：

```
static struct platform_device *crag6410_devices[] __initdata = {
        ......
        &s3c64xx_device_spi0,
        ......
};
 
static void __init xxxx_machine_init(void)
{
 
        s3c64xx_spi0_set_platdata(NULL, 0, 2);
        //注册平台设备
        platform_add_devices(crag6410_devices, ARRAY_SIZE(crag6410_devices));
}
```
s3c64xx_spi0_set_platdata函数的定义如下:

```c
void __init s3c64xx_spi0_set_platdata(int (*cfg_gpio)(void), int src_clk_nr,
						int num_cs)
{
	struct s3c64xx_spi_info pd;
	......
	pd.num_cs = num_cs;
	pd.src_clk_nr = src_clk_nr;
	pd.cfg_gpio = (cfg_gpio) ? cfg_gpio : s3c64xx_spi0_cfg_gpio;
        ......
	s3c_set_platdata(&pd, sizeof(pd), &s3c64xx_device_spi0);
}
```
上述函数主要是指定了控制器使用到的gpio配置、片选引脚个数和时钟配置等信息。这些信息在后面的控制器驱动中要使用到。

# 注册SPI控制器的platform_driver
上一节中，我们把SPI控制器注册为一个platform_device，相应地，对应的驱动就应该是一个平台驱动：platform_driver，它们通过platform bus进行相互匹配。以下的代码来自：`/drivers/spi/spi-s3c64xx.c`

```c
static struct platform_driver s3c64xx_spi_driver = {
        .driver = {
                .name   = "s3c64xx-spi",
                .owner = THIS_MODULE,
                .pm = &s3c64xx_spi_pm,
                .of_match_table = of_match_ptr(s3c64xx_spi_dt_match),
        },
        .remove = s3c64xx_spi_remove,
        .id_table = s3c64xx_spi_driver_ids,
};
MODULE_ALIAS("platform:s3c64xx-spi");
 
static int __init s3c64xx_spi_init(void)
{
        return platform_driver_probe(&s3c64xx_spi_driver, s3c64xx_spi_probe);
}
subsys_initcall(s3c64xx_spi_init);
```
显然，系统初始化阶段（subsys_initcall阶段），通过`s3c64xx_spi_init()`，注册了一个平台驱动，该驱动的名字正好也是：s3c64xx-spi，自然地，平台总线会把它和上一节定义的platform_device匹配上，并且触发probe回调被调用（就是s3c64xx_spi_probe函数）。当然，这里的匹配是通过id_table字段完成的：

```
static struct platform_device_id s3c64xx_spi_driver_ids[] = {
        {
                .name           = "s3c2443-spi",
                .driver_data    = (kernel_ulong_t)&s3c2443_spi_port_config,
        }, {
                .name           = "s3c6410-spi",
                .driver_data    = (kernel_ulong_t)&s3c6410_spi_port_config,
        }, 
        ......
        { },
};
```

# 注册spi_master
在linux设备模型看来，代表SPI控制器的是第一节所定义的`platform_device`结构，但是对于SPI通用接口层来说，代表控制器的是`spi_master`结构，关于spi_master结构的描述，请参看`第二篇文章：Linux SPI总线和设备驱动架构之二：SPI通用接口层`。我们知道，设备和驱动匹配上后，驱动的probe回调函数就会被调用，而probe回调函数正是对驱动程序和设备进行初始化的合适时机，本例中，对应的probe回调是：s3c64xx_spi_probe：

```c
static int s3c64xx_spi_probe(struct platform_device *pdev)
{
        ......
 
        /* 分配一个spi_master结构 */
        master = spi_alloc_master(&pdev->dev,
                                sizeof(struct s3c64xx_spi_driver_data));
        ......
 
        platform_set_drvdata(pdev, master);
        ......
        master->dev.of_node = pdev->dev.of_node;
        master->bus_num = sdd->port_id;
        master->setup = s3c64xx_spi_setup;
        master->cleanup = s3c64xx_spi_cleanup;
        master->prepare_transfer_hardware = s3c64xx_spi_prepare_transfer;
        master->transfer_one_message = s3c64xx_spi_transfer_one_message;
        master->unprepare_transfer_hardware = s3c64xx_spi_unprepare_transfer;
        master->num_chipselect = sci->num_cs;
        master->dma_alignment = 8;
        master->bits_per_word_mask = SPI_BPW_MASK(32) | SPI_BPW_MASK(16) |
                                        SPI_BPW_MASK(8);
        /* the spi->mode bits understood by this driver: */
        master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_CS_HIGH;
        master->auto_runtime_pm = true;
 
        ......
        /* 向通用接口层注册spi_master结构 */
        if (spi_register_master(master)) {
                dev_err(&pdev->dev, "cannot register SPI master\n");
                ret = -EBUSY;
                goto err3;
        }
 
        ......
}
```
上述函数，除了完成必要的硬件资源初始化工作以外，最重要的工作就是通过spi_alloc_master函数分配了一个spi_master结构，初始化该结构，最终通过spi_register_master函数完成了对控制器的注册工作。从代码中我们也可以看出，spi_master结构中的几个重要的回调函数已经被赋值，这几个回调函数由通用接口层在合适的时机被调用，以便完成控制器和设备之间的数据交换工作。

# 实现spi_master结构的回调函数


事实上，SPI控制器驱动程序的主要工作，就是要实现spi_master结构中的几个回调函数，其它的工作逻辑，均由通用接口层帮我们完成，通用接口层会在适当的时机调用这几个回调函数，这里我只是介绍一下各个回调函数的作用，具体的实现例子，请各位自行阅读代码树中各个平台的例子（代码位于：`/drivers/spi/`）。

## int (*setup)(struct spi_device *spi)
当协议驱动希望修改控制器的工作模式或参数时，会调用通用接口层提供的API：`spi_setup()`，该API函数最后会调用`setup回调函数`来完成设置工作。

## int (*transfer)(struct spi_device *spi, struct spi_message *mesg)
目前已经可以不用我们自己实现该回调函数，初始化时直接设为NULL即可，目前的通用接口层已经实现了消息队列化，注册spi_master时，通用接口层会提供实现好的通用函数。现在只有一些老的驱动还在使用该回调方式，新的驱动应该停止使用该回调函数，而是应该使用队列化的transfer_one_message回调。需要注意的是，我们只能选择其中一种方式，设置了transfer_one_message回调，就不能设置transfer回调，反之亦然。

## void (*cleanup)(struct spi_device *spi)
当一个SPI从设备（spi_device结构）被释放时，该回调函数会被调用，以便释放该从设备所占用的硬件资源。

## int (*prepare_transfer_hardware)(struct spi_master *master)
## int (*unprepare_transfer_hardware)(struct spi_master *master)
这两个回调函数用于在发起一个数据传送过程前和后，给控制器驱动一个机会，申请或释放某些必要的硬件资源，例如DMA资源和内存资源等等。

## int (*prepare_message)(struct spi_master *master, struct spi_message *message)
## int (*unprepare_message)(struct spi_master *master, struct spi_message *message)
这两个回调函数也是用于在发起一个数据传送过程前和后，给控制器驱动一个机会，对message进行必要的预处理或后处理，比如根据message需要交换数据的从设备，设定控制器的正确工作时钟、字长和工作模式等。

## int (*transfer_one_message)(struct spi_master *master, struct spi_message *mesg)
当通用接口层发现master的队列中有消息需要传送时，会调用该回调函数，所以该函数是真正完成一个消息传送的工作函数，当传送工作完成时，应该调用spi_finalize_current_message函数，以便通知通用接口层，发起队列中的下一个消息的传送工作。
