# 三、编写与regulator 设备树节点对应的driver

1. 这些driver的存在形式是多种多样的，但所做的工作基本类似：

* 初始化regulator的宿主（如tps5163、PMIC、等等），最终的目的是，通过宿主提供的接口，修改regulator的输出。
* 初始化用于描述regulator的静态信息（struct regulator_desc）和动态信息（struct regulator_config），并以这二者为参数，调用regulator_register接口，将regulator注册到kernel中。
* 静态信息中包含regulator的操作函数集（struct regulator_ops），后续regulator的控制，将会由regulator framework core直接调用这些回调函数完成。
* 后面的事情，例如sysfs attribute创建等，就交给regulator framework core了。

2. 相关结构体

(1) struct regulator_init_data

```c
struct regulator_init_data {
    /*该regulator的前级regulator，一般在regulator driver中直接指定*/
    const char *supply_regulator;
    /*该regulator的使用限制，由DTS配置，并可以借助regulator core提供的辅助
      API（regulator_of_get_init_data）自动解析 
    */
    struct regulation_constrains constrains;

    /*
    使用该regulator的consumer的个数，及其设备名和supply名的map。 
    用于建立consumer设备和regulator之间的关联.
    */
    int num_consumer_supplies;
    struct regulator_consumer_supply *consumer_supplies;

    /*可选的特定于机器的初始化函数，是regulator的init回调，
    由regulator driver提供，并在regulator注册时调用。
    */
    int (*regulator_init)(void *driver_data);
    /* 保存driver的私有数据，并在调用regulator_init时传入 */
    void *driver_data; /*core does not touch this*/
};
```
对regulator driver而言，

**DTS主要用于配置regulator的init data**(struct regulator_init_data)。

(2) struct regulation_constraints 

```c
struct regulation_constraints {
    /*描述该constrains*/
    const char *name;

    /*输出范围(uV)*/
    int min_uV;
    int max_uV;

    /*consumer看到的电压和实际电压之间的偏移值。通常 用于补偿压降。
    只对voltage regulator 有效*/
    int uV_offset;

    /*输出电流范围:uA,只对current regulator有效*/
    int min_uA;
    int max_uA;
    int ilim_uA;

    int system_load;
    /* valid regulator operating modes for this machine*/
    unsigned int valid_modes_mask;

    /*
    该regulator支持哪些操作，以bit mask的形式提供，包括
        REGULATOR_CHANGE_VOLTAGE，可以改变输出电压；
        REGULATOR_CHANGE_CURRENT，可以改变输出电流； 
        REGULATOR_CHANGE_MODE，可以修改mode； 
        REGULATOR_CHANGE_STATUS，可以enable/disable； 
        REGULATOR_CHANGE_DRMS，支持Dynamic Regulator Mode Switching（DRMS），可以动态的调整regulator的mode. 
        REGULATOR_CHANGE_BYPASS，支持bypass模式。
    */
    unsigned int valid_ops_mask;

    /*
        如果该regulator的输入是另一个regulator，该字段指定regulator期望的输入电压；
         regulator input voltage - only if supply is another regulator
    */
    int input_uV;

    /*
     * regulator电源管理有关的字段
     * regulator suspend states for global PMIC STANDBY/HIBERNATE
     */
    struct regulator_state state_disk;
    struct regulator_state state_mem;
    struct regulator_state state_standby;
    suspend_state_t initial_state; /* suspend state to set at init */

    /* 初始mode mode to set on startup */
    unsigned int initial_mode;
    /*
     * 由于模拟器件的特性，电压改变，需要一定的生效时间。在一定的范围内，生效时间和电
     * 压的变化值成比例。该变量就是描述regulator器件的这个特性，单位为uV/us，即1us可
     * 以产生多大的电压变化。在rapm_disable不为1的情况下，当consumer要求改变电压时，
     * regulator framework core会根据该变量，以及电压改变量，计算出需要等待的时间，
     * 进行延时操作.
     */
     unsigned int ramp_delay;
     /*
      * regulator的开启时间，单位为us。consumer enable regulator时，
      * regulator framework会根据该变量进行延时操作
      */
      unsigned int enable_time;

      /* 是否一直保持使能状态 */
      unsigned always_on:1;    /* regulator never off when system is on */
      /* 是否在启动时使能 */
      unsigned boot_on:1;    /* bootloader/firmware enabled regulator */
      /*
       * 如果min_uV和max_uV相同，该变量指示“在regulator注册到kernel时，
       * 是否将电压设置为min_uV/max_uV。 
       *                                                                                        */
       unsigned apply_uV:1;    /* apply uV constraint if min == max */
                                                                                              /* 是否禁止延时操作 */
       unsigned ramp_disable:1; /* disable ramp delay */
       unsigned soft_start:1;    /* ramp voltage slowly */
       unsigned pull_down:1;    /* pull down resistor when regulator off */
       unsigned over_current_protection:1; /* auto disable on over current */
};
```

DTS的内容都在struct regulation_constraints中，该结构保存了该regulator所有的物理限制

结合struct regulation_constraints结构，我们解释DTS：

```dts
tps51632@43 {
    compatible = "ti, tps51632";
    reg = <0x43>;
    regulator-name = "vdd-cpu";
    regulator-min-microvolt = <500000>;
    regulator-max-microvolt = <1520000>;
    regulator-boot-on;
    regulator-always-on;
};
```

* regulator-name，对应struct regulation_constraints中name；
* regulator-min-microvolt，对应struct regulation_constraints中的min_uV；
* regulator-max-microvolt，对应struct regulation_constraints中的max_uV；
* regulator-boot-on，对应struct regulation_constraints中的boot_on；
* regulator-always-on，对应struct regulation_constraints中的always_on。
* 其它的字段，可以根据实际情况，自行添加，具体可参考“Documentation/devicetree/bindings/regulator/regulator.txt”中的描述。

(3) struct regulator_desc

```c
struct regulator_desc {
    /*regulator名称,唯一标识regulator*/
    const char *name;

    /*该regulator输入，regulator名称*/
    const char *supply_name;
    const char *of_match;
    const char *regulators_node;
    int (*of_parse_cb)(struct device_node *, const struct regulator_desc *, struct regulator_config *);

    /*标识该regulator的一个数字*/
    int id;

    /*true时，表示该regulator可以定范围连续的电压*/
    bool continuous_voltage_range;

    /*consumer可以通过ops.list_voltage()接口，获取该regulator可以输出的电压值*/
    /*该变量指定可以获取的电压值的个数*/
    unsigned n_voltages;

    /*该regulator的操作函数集*/
    const struct regulators_ops *ops;
    /*该regulator的中断号(有的话)*/
    int irq;

    /*该regulator的类型，包括REGULATOR_VOLTAGE和REGULATOR_CURRENT两种 */
    enum regulator_type type;
    struct module *owner;
    unsigned int min_uV;
    unsigned int uV_step;
    unsigned int linear_min_sel;
    int fixed_uV;
    unsigned int ramp_delay;
    int min_dropout_uV;
    
    const struct regulator_linear_range *linear_ranges;
    int n_linear_ranges;

    const unsigned int *volt_table;

    unsigned int vsel_reg;
    unsigned int vsel_mask;
    unsigned int apply_reg;
    unsigned int apply_bit;
    unsigned int enable_reg;
    unsigned int enable_mask;
    unsigned int enable_val;
    unsigned int disable_val;
    bool enable_is_inverted;
                                                                                    
    unsigned int bypass_reg;
    unsigned int bypass_val_on;
                                                                                           unsigned int bypass_val_off;
                                                                                           unsigned int enable_time;
                                                                                           unsigned int off_on_delay;
                                                                                           unsigned int (*of_map_mode)(unsigned int mode);

};
```
在注册regulator的时候，需要使用 struct regulator_desc 结构提供该regulator的静态描述。所谓的静态，是指这些描述不会在运行时改变，代表了设备的一种属性，

(4) struct regulator_config

```c
struct regulator_config {
    /*会在regulator_register时，由regulator core分配，保存在此，以便后续使用*/
    struct device *dev;
    /*init data指针，在解析DTS后，保存在此，以便后续使用*/
    const struct regulator_init_data *init_data;
    void *driver_data;
    struct device_node *of_node;
    struct regmap *regmap;

    bool ena_gpio_initialized;
    int ena_gpio;
    unsigned int ena_gpio_invert:1;
    unsigned int ena_gpio_flags;
};
```
struct regulator_config保存了regulator的动态信息，所谓的动态信息，是指那些会在driver运行过程中改变、或者driver运行后才会确定的信息.

(5) struct regulator_dev

```c
struct regulator_dev {
    const struct regulator_desc *desc;
    int exclusive;
    u32 use_count;
    u32 open_count;
    u32 open_offset;
    u32 bypass_count;

/* lists we belong to */
    struct list_head list; /* list of all regulators */
    /* lists we own */
    struct list_head consumer_list; /* consumers we supply */
    struct blocking_notifier_head notifier;
    struct mutex mutex; /* consumer lock */
    struct module *owner;
    struct device dev;
    struct regulation_constraints *constraints;
    struct regulator *supply;    /* for tree */
    const char *supply_name;
    struct regmap *regmap;

    struct delayed_work disable_work;
    int deferred_disables;

    void *reg_data;        /* regulator_dev data */

    struct dentry *debugfs;

    struct regulator_enable_gpio *ena_pin;
    unsigned int ena_gpio_state:1;

    /* time when this regulator was disabled last time */
    unsigned long last_off_jiffy;
    struct proxy_consumer *proxy_consumer;
    struct regulator *debug_consumer;
```

struct regulator_dev是regulator设备的抽象，当driver以struct regulator_desc、struct regulator_config两个类型的参数，调用regulator_register将regulator注册
到kernel之后，regulator就会分配一个struct regulator_dev变量，后续所有的regulator操作，都将以该变量为对象。

3.regulator的操作模式（operation mode）

regulator的主要功能，是输出`电压/电流`的调整。由于模拟器件的特性，`电压/电流`的改变，是需要一定的时间的。对有些regulator而言，可以工作在不同的模式，这些模式
有不同的改变速度（一般较快的速度，有较大的功耗）。下面是operation mode定义（位于`include/linux/regulator/consumer.h`中）：

```c
#define REGULATOR_MODE_FAST            0x1
#define REGULATOR_MODE_NORMAL        0x2
#define REGULATOR_MODE_IDLE            0x4
#define REGULATOR_MODE_STANDBY        0x8
```
相应的，regulator framework 提供了一些机制，用于 operation mode 的操作，包括：
(1) struct regulation_constraints 中用于表示初始模式的字段 initial_mode。
(2) regulator ops中的`set_mode/get_mode`回调函数。

4. kernel抽象了两种电压操作的方法：

(1) 直接操作电压，对应struct regulator_ops中的list_voltage、get_voltage、set_voltage回调函数：

set_voltage：用于将电压设置为min_uV和max_uV范围内、和min_uV最接近的电压。该接口可以返回一个selector参数，用于告知调用者，实际的电压值；

* get_voltage：用于返回当前的电压值；
* list_voltage：以selector为参数，获取对应的电压值。

(2) selector的形式

regulator driver以selector的形式，反映电压值。selector是一个从0开始的整数，driver提供的相关接口为struct regulator_ops中的list_voltage、map_voltage、
set_voltage_sel、get_voltage_sel回调函数。

* map_voltage: 是和list_voltage相对的接口，用于将电压范围map成一个selector值；
* set_voltage_sel/get_voltage_sel：以selector的形式，操作电压。

regulator driver可以根据实际情况，选择一种实
