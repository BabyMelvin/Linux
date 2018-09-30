Android9 在之前迭代车辆HAL弃用`AUDIO_*`属性，用专用的Audio Control HAL层控制包括函数调用和类型列表。

新的HAL将`IAudioControl`暴露为原始接口对象，它提供入口点和车辆音频引擎交互，可以配置和音量控制。系统能够包含一个具体对象实例，该实例通过`CarAudioService`在构造的时间建立。这个对象是传统Audio HAL的扩展。多数的实现，统一进程提供Audio HAL 应该提供`IAudioControl`接口

# 1.支持接口
这个`AudioControl`HAL支持如下接口：

* `getBusforContext`:每个context起来的调用来获得`ContextNumber`与`busAddress`映射。使用：

```java
getBusForContext(ContextNumber contextNumber)
generates(uint32_t busNumber)
```
对每个context，让车辆告诉框架物理输出流路由到哪里。每个context，一个有限bus数字将会被返回(0-num busses-1).如果不认识`contextNumber`，应该返回-1.任何返回的无效的`busNumber`将会路由到bus0.

一个并发的声音和同一个`busNumber`关联，在被作为一个单独留送到Audio HAL之前，这个机制通过Android的`AudioFlinger`进行混合。这就去掉了车辆属性`AUDIO_HW_VARIANT`和`AUDIO_ROUTING_POLICY`

* `setBalanceTowardRight`:控制左/右平衡配置车辆扬声器：`setBalanceTowardRight(float value)`.改变扬声器音量右(+1)或者左(-1)当car是0.0，+1.0是全右，-1.0是全左，一个在`[-1,1]`区间外是错误的。

* `setFadeTowardFront`:控制前或后车辆扬声器的fade：`setFadeTowardFront(float value)`.移动范围和balance一样。


# 2.配置音量
