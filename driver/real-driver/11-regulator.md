# 一.Regulator 简介

1.Regulator，中文名翻译为“稳定器”，在电子工程中，是voltage regulator（稳压器）或者current regulator（稳流器）的简称，指可以自动维持恒定电压（或电流）的装置。从设备驱动的角度看，regulator的控制应该很简单

* 输出的enable/disable
* 输出电压或电流的**大小的控制**

软件架构分consumer、provider、core

Linux regulator framework的目的：提供标准的内核接口，控制系统的voltage/current regulators，并提供相应的机制，在系统运行的过程中，动态改变regulators的输出，以达到省电的目的

## 2.regulator driver

driver模块的功能，是抽象regulator设备。可以参考 qcom_smd-regulator.c

(1) 使用`struct regulator_desc`描述regulator的静态信息，包括：

* 名字
* supply regulator 的名字
* 中断号
* 操作函数集（struct regulator_ops）
* 使用regmap时相应的寄存器即 bitmap 等等。

(2) 使用 struct regulator_config，描述 regulator 的动态信息（所谓的动态信息，体现在`struct regulator_config`变量都是局部变量，因此不会永久保存），包括`struct regulator_init_data`指针、设备指针、enable gpio等等。

(3) 提供 regulator 的注册接口（regulator_register/devm_regulator_register），该接口接受描述该 regulator 的两个变量的指针：struct regulator_desc和struct regulator_config，并分配一个新的数据结构（struct regulator_dev，从设备的角度描述regulator），并把静态指针（struct regulator_desc）和动态指针（struct regulator_config）提供的信息保存在其中。

(4) 最后，regulator driver 将以为`struct regulator_dev`指针为对象，对 regulator 进行后续的操作。`struct regulator_dev *rdev;` regulator deriver对regulator的抽象

## 3.consumer
consumer 的功能，**是从regulator consumer的角度**，抽象 regulator 设备（struct regulator 是对 consumer 设备的抽象描述），并提供 regulator 操作相关的接口。
struct regulator consumer视角对regulator的抽象。

## 4.core
core负责上述逻辑的具体实现，并以sysfs的形式，向用户空间提供接口。

# 二、相关接口介绍
1. regulator framework 向内核空间 consumer 提供的接口位于“include/linux/regulator/consumer.h”中，包括 regulator 的获取、使能、修改等接口，如下。

(1) struct regulator

struct regulator结构用于从 consumer 的角度抽象一个regulator，consumer不需要关心该结构的细节，当作一个句柄使用即可。

(2) regulator的`get/put`接口

```c
struct regulator *__must_check regulator_get(struct device *dev, const char *id);
struct regulator *__must_check devm_regulator_get(struct device *dev, const char *id);
struct regulator *__must_check regulator_get_exclusive(struct device *dev, const char *id);
struct regulator *__must_check devm_regulator_get_exclusive(struct device *dev, const char *id);
struct regulator *__must_check regulator_get_optional(struct device *dev, const char *id);
struct regulator *__must_check devm_regulator_get_optional(struct device *dev, const char *id);
void regulator_put(struct regulator *regulator);
void devm_regulator_put(struct regulator *regulator);
```

(3)根据是否独占regulator、是否可以多次get，regulator get接口分为三类：

正常的get，非独占、可以重复get，`regulator_get/devm_regulator_get`
独占性质的get，独占、不可重复get，`regulator_get_exclusive/devm_regulator_get_exclusive`
optional的get，非独占、不可重复get，`regulator_get_optional/devm_regulator_get_optional`。

(4) supply alias相关的接口

```c
int regulator_register_supply_alias(struct device *dev, const char *id, struct device *alias_dev, const char *alias_id);
void regulator_unregister_supply_alias(struct device *dev, const char *id);
int devm_regulator_register_supply_alias(struct device *dev, const char *id, struct device *alias_dev, const char *alias_id);
void devm_regulator_unregister_supply_alias(struct device *dev, const char *id);
int devm_regulator_bulk_register_supply_alias(struct device *dev, const char *const *id, struct device *alias_dev, const char *const *alias_id, nt num_id);
void devm_regulator_bulk_unregister_supply_alias(struct device *dev, const char *const *id, int num_id);
``nt regulator_register_supply_alias(struct device *dev, const char *id, struct device *alias_dev, const char *alias_id);
void regulator_unregister_supply_alias(struct device *dev, const char *id);
int devm_regulator_register_supply_alias(struct device *dev, const char *id, struct device *alias_dev, const char *alias_id);
void devm_regulator_unregister_supply_alias(struct device *dev, const char *id);
int devm_regulator_bulk_register_supply_alias(struct device *dev, const char *const *id, struct device *alias_dev, const char *const *alias_id, nt num_id);
void devm_regulator_bulk_unregister_supply_alias(struct device *dev, const char *const *id, int num_id);
```

(5) regulator的控制、状态获取接口

```c
int __must_check regulator_enable(struct regulator *regulator);
int regulator_disable(struct regulator *regulator);
int regulator_force_disable(struct regulator *regulator);
int regulator_is_enabled(struct regulator *regulator);
int regulator_disable_deferred(struct regulator *regulator, int ms);

int regulator_can_change_voltage(struct regulator *regulator);
int regulator_count_voltages(struct regulator *regulator);
int regulator_list_voltage(struct regulator *regulator, unsigned selector);
int regulator_is_supported_voltage(struct regulator *regulator, int min_uV, int max_uV);
unsigned int regulator_get_linear_step(struct regulator *regulator);
int regulator_set_voltage(struct regulator *regulator, int min_uV, int max_uV);
int regulator_set_voltage_time(struct regulator *regulator, int old_uV, int new_uV);
int regulator_get_voltage(struct regulator *regulator);
int regulator_sync_voltage(struct regulator *regulator);
int regulator_set_current_limit(struct regulator *regulator, int min_uA, int max_uA);
int regulator_get_current_limit(struct regulator *regulator);

int regulator_set_mode(struct regulator *regulator, unsigned int mode);
unsigned int regulator_get_mode(struct regulator *regulator);
int regulator_set_optimum_mode(struct regulator *regulator, int load_uA);

int regulator_allow_bypass(struct regulator *regulator, bool allow);

struct regmap *regulator_get_regmap(struct regulator *regulator);
int regulator_get_hardware_vsel_register(struct regulator *regulator, unsigned *vsel_reg, unsigned *vsel_mask);
int regulator_list_hardware_vsel(struct regulator *regulator, unsigned selector);
```
控制有关的包括enable、disable、电压设置、电流设置、mode设置、状态获取等，其中disable又包括normal、强制、退出等类型。
状态获取包括：是否enable，是否可以改变电压，支持的电压列表，是否支持指定范围的电压，当前输出电压，当前电流限制，当前mode，等等。

(7) inotifier相关的接口

```c
int regulator_register_notifier(struct regulator *regulator, struct notifier_block *nb);
int regulator_unregister_notifier(struct regulator *regulator, struct notifier_block *nb);
```
如果consumer关心某个regulator的状态变化，可以通过上面接口注册一个notifier。

(8) 其它接口

```c
/* driver data - core doesn't touch */
void *regulator_get_drvdata(struct regulator *regulator);
void regulator_set_drvdata(struct regulator *regulator, void *data);
```
用于设置和获取driver的私有数据。

2.consumer 模块向用户空间 consumer 提供的接口

用户空间程序可以通过sysfs接口，使用regulator，就像内核空间consumer一样，这些接口由“`drivers/regulator/userspace-consumer.c`”实现，主要包括：

* sysfs目录位置：/sys/devices/platform/reg-userspace-consumer。
    * name：读取可以获取该regulator的名字。
    * state：读取，可以获取该regulator的状态（enabled/disabled）；写入可以改变regulator的状态（enabled或者1使能，disabled或者0禁止）。

3.machine 模块向 regulator driver 提供的接口

machine 模块主要提供`struct regulator_init_data`、`struct regulation_constraints constraints` 等数据结构，用于描述板级的 regulator 配置。

4. regulator framework 模块向 regulator driver 提供的接口

regulator framework 向 regulator driver 提供的接口位于“include/linux/regulator/driver.h”中，包括数据结构抽象、regulator注册等。

(1) 数据结构抽象有 struct regulator_desc、struct regulator_config 和 struct regulator_dev
(2) regulator设备的注册接口

```c
struct regulator_dev * regulator_register(const struct regulator_desc *regulator_desc, const struct regulator_config *config);
struct regulator_dev * devm_regulator_register(struct device *dev, const struct regulator_desc *regulator_desc, const struct regulator_config *config);
void regulator_unregister(struct regulator_dev *rdev);
void devm_regulator_unregister(struct device *dev, struct regulator_dev *rdev);
```
5. core模块向用户空间提供的sysfs接口

regulator设备在内核中是以regulator class的形式存在的，regulator core通过class->dev_groups的方式，提供了一些默认的attribute，包括：

* name，读取可以获取该regulator的名字；
* num_users，读取可获取regulator的使用者数目；
* type，读取可以获取该regulator的类型（voltage或者current）。

另外，如果regulator driver需要提供更多的attribute（如状态、最大/最小电压等等），可以调用add_regulator_attributes接口，主动添加。位于`/sys/class/regulator/.../`目录下，具体可参考：https://www.kernel.org/doc/Documentation/ABI/testing/sysfs-class-regulator。
