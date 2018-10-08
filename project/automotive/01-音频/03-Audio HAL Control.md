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
Android车辆实现应该控制音量使用硬件而不是软件混合。避免副作用，将`device/generic/car/emulator/audio/overlay/frameworks/base/core/res/res/values/config.xml, set the config_useFixedVolume`设为`true`(必要覆盖)

```xml
<resources>
    <!-- Car uses hardware amplifier for volume. -->
    <bool name="config_useFixedVolume">true</bool>
</resources>
```

当`config_useFixedVolume`没有设置，或者设置为`false`，应用能够调用`AudioManager.setStreamVolume()`和通过在软混合器中流类型来改变音量。这个不是想要的，因为会对其他应用有潜在的影响，并且当接收到硬件放大，在软混合中会音量衰减由于在信号中获得某些重要位。

# 3.配置音量组

`CarAudioService`使用定义在`packages/services/Car/service/res/xml/car_volume_group.xml`中的音量组。可以自己重定义来覆盖这个文件。组是在runtime定义的通过在XML文件中的顺序。IDs范围是从0到N-1，N代表音量组的数目.

```xml
<volumeGroups xmlns:car="http://schemas.android.com/apk/res-auto">
    <group>
        <context car:context="music"/>
        <context car:context="call_ring"/>
        <context car:context="notification"/>
        <context car:context="system_sound"/>
    </group>
    <group>
        <context car:context="navigation"/>
        <context car:context="voice_command"/>
    </group>
    <group>
        <context car:context="call"/>
    </group>
    <group>
        <context car:context="alarm"/>
    </group>
</volumeGroups>
```
The attributes used in this configuration are defined in`packages/services/Car/service/res/values/attrs.xml`

# 4.处理音量按键事件
Android定义了几个控制音量的键码值，如`KEYCODE_VOLUME_UP`,`KEYCODE_VOLUME_DOWN`和`KEYCODE_VOLUME_MUTE`.默认，Android路由音量键值给应用。车辆实现应该**强制**这些事件给`CarAudioService`,它能够恰当的调用`setGroupVolume`和`setMasterMute`.

**实现这个行为**:将`device/generic/car/emulator/car/overlay/frameworks/base/core/res/res/values/config.xml`中的`config_handleVolumeKeysInWindowManager`设置成`true`

```xml
<resources>
    <bool name="config_handleVolumeKeysInWindowManager">true</bool>
</resources>
```

# 5.CarAudioManager API

`CarAudioManager`使用`CarAudioService`来配置和控制车辆音频系统的。这个manager对系统中的apps是不可见的，但是对特殊组件可见，如音量控制器，能够使用`CarAudioManager`和系统交互。

下面描述Android 9对`CarAudioManager`API的改变。

## 5.1 不建议使用APIs
Android 9处理设备枚举通过已有的`AudioManager.getDeviceList`接口，所以下面的函数不建议使用被移除了：

* `String[] getSupportedExternalSourceTypes()`
* `String[] getSupportedRadioTypes()`

Andriod 9处理音量使用`AudioAttributes.AttributeUsage`或者音量组入口点，所以下面的APIs音量`streamType`被移除了：

* `void setStreamVolume(int streamType, int index, int flags)`
* `int getStreamMaxVolume(int streamType)`
* `int getStreamMinVolume(int streamType)`
* `void setVolumeController(IVolumeController controller)`
## 5.2 新的APIs
Android 9 新增接口控制硬件外放(明确基于音量组):

* `int getVolumeGroupIdForUsage(@AudioAttributes.AttributeUsage int usage)`
* `int getVolumeGroupCount()`
* `int getGroupVolume(int groupId)`
* `int getGroupMaxVolume(int groupId)`
* `int getGroupMinVolume(int groupId)`

另外，Android 9还提供新的接口给系统GUI：

* `void setGroupVolume(int groupId, int index, int flags)`
* `void registerVolumeChangeObserver(@NonNull ContentObserver observer)`
* `void unregisterVolumeChangeObserver(@NonNull ContentObserver observer)`
* `void registerVolumeCallback(@NonNull IBinder binder)`
* `void unregisterVolumeCallback(@NonNull IBinder binder)`
* `void setFadeToFront(float value)`
* `Void setBalanceToRight(float value)`

最后，Android 9新增对额外资源的管理。这些主要目的为了支持基于媒体类型路由外部资源到输出buses。同样间接让第三方应用获得外部资源：

* `String[] getExternalSources()`:放回地址数组，类型标识音频接口流类型`AUX_LINE`,`FM_TUNER`,`TV_TUNER`和`BUS_INPUT`
* `CarPatchHandle createAudioPatch(String sourceAddress, int carUsage)`:路由资源地址到输出BUS，这个BUS和提供的`carUsage`相关.
* `int releaseAudioPatch(CarPatchHandle patch)`删除提供的patches。如果`CarPatchHandle`意外死亡，这个由`AudioPolicyService::removeNotificationClinet()`自动完成。

# 6.创建音频patches
能够在两个音频端口之间创建一个patch，或者一个mix端口和一个设备端口。一般，一个音频patch从一个mix到一个设备端口是播放的，相反的方向是获取的。

例如一个音频patch路由音频样本从`FM_TUNER`源直接到一个media sink跨国软件混合器。必须使用一个硬件混合器从Android和`FM_TUNER`为sink进行混合。当直接从`FM_TUNER`到媒体sink直接创建一个音频patch：

* 在媒体sink用的音频控制并且应该影响Android和`FM_TUNER`音频
* 用户应该能够通过一个采样app在Android和`FM_TUNER`之间切换(不是明确meida资源选项是必要的)

车辆实现应该需要在两个设备端口建立一个音频patch。为了这个应该首先声明设备端口和可能的路由在`audio_policy_configuration.xml`和设备相关的mixports

## 6.1 配置示例
参照：`device/generic/car/emulator/audio/audio_policy_configuration.xml`

```xml
<audioPolicyConfiguration>
    <modules>
        <module name="primary" halVersion="3.0">
            <attachedDevices>
                <item>bus0_media_out</item>
                <item>bus1_audio_patch_test_in</item>
            </attachedDevices>
            <mixPorts>
                <mixPort name="mixport_bus0_media_out" role="source"
                        flags="AUDIO_OUTPUT_FLAG_PRIMARY">
                    <profile name="" format="AUDIO_FORMAT_PCM_16_BIT"
                            samplingRates="48000"
                            channelMasks="AUDIO_CHANNEL_OUT_STEREO"/>
                </mixPort>
                <mixPort name="mixport_audio_patch_in" role="sink">
                    <profile name="" format="AUDIO_FORMAT_PCM_16_BIT"
                           samplingRates="48000"
                           channelMasks="AUDIO_CHANNEL_IN_STEREO"/>
                </mixPort>
            </mixPorts>
            <devicePorts>
                <devicePort tagName="bus0_media_out" role="sink" type="AUDIO_DEVICE_OUT_BUS"
                        address="bus0_media_out">
                    <profile balance="" format="AUDIO_FORMAT_PCM_16_BIT"
                            samplingRates="48000" channelMasks="AUDIO_CHANNEL_OUT_STEREO"/>
                    <gains>
                        <gain name="" mode="AUDIO_GAIN_MODE_JOINT"
                                minValueMB="-8400" maxValueMB="4000" defaultValueMB="0" stepValueMB="100"/>
                    </gains>
                </devicePort>
                <devicePort tagName="bus1_audio_patch_test_in" type="AUDIO_DEVICE_IN_BUS" role="source"
                        address="bus1_audio_patch_test_in">
                    <profile name="" format="AUDIO_FORMAT_PCM_16_BIT"
                            samplingRates="48000" channelMasks="AUDIO_CHANNEL_IN_STEREO"/>
                    <gains>
                        <gain name="" mode="AUDIO_GAIN_MODE_JOINT"
                                minValueMB="-8400" maxValueMB="4000" defaultValueMB="0" stepValueMB="100"/>
                    </gains>
                </devicePort>
            </devicePorts>
            <routes>
                <route type="mix" sink="bus0_media_out" sources="mixport_bus0_media_out,bus1_audio_patch_test_in"/>
                <route type="mix" sink="mixport_audio_patch_in" sources="bus1_audio_patch_test_in"/>
            </routes>
        </module>
    </modules>
</audioPolicyConfiguration>
```

## 6.2 音频驱动API
能够使用`getExternalSources()`来获得可获得资源的列表(地址来识别),然后再资源之间建立patch和通过音频使用sink端口。相对应的Audio HAL入口位置在`IDevice.hal`:

```xml
Interface IDevice {
...
/**
* Creates an audio patch between several source and sink ports.  The handle
* is allocated by the HAL and must be unique for this audio HAL module.
*
* @param sources patch sources.
* @param sinks patch sinks.
* @return retval operation completion status.
* @return patch created patch handle.
*/
createAudioPatch(vec<AudioPortConfig> sources, vec<AudioPortConfig> sinks)
       generates (Result retval, AudioPatchHandle patch);

/**
* Release an audio patch.
*
* @param patch patch handle.
* @return retval operation completion status.
*/
releaseAudioPatch(AudioPatchHandle patch) generates (Result retval);
...
}
```

**注意**：从`AUDIO_DEVICE_API_VERSION_3_0`API就能够获得了。细节参考：`device/generic/car/emulator/audio/driver/audio_hw.c`

# 7.配置音量设置UI

Android 9将音量设置UI从音量配置组解耦出来(它能够在配置音量组覆盖作为描述)。这个分隔确保当音量组配置改变也不需要进行变化。

在汽车设置UI，`packages/apps/Car/Settings/res/xml/car_volume_items.xml`包含UI元素(title和icon资源)和每个定义在`AudioAttributes.USAGE`相关联。这个文件为了一个合理渲染而提供，定义在VolumeGroups中，通过使用和在每个VolumeGroup第一个使用usage关联资源.

例如，下面定义一个VolumeGroup，其中包含`voice_communication`和`voice_communication_signalling`.汽车设置UI默认的实现来渲染VolumeGroup,使用和`voice_communication`在文件中第一个匹配的相关的资源。

```xml
<carVolumeItems xmlns:car="http://schemas.android.com/apk/res-auto">
    <item car:usage="voice_communication"
          car:title="@*android:string/volume_call"
          car:icon="@*android:drawable/ic_audio_ring_notif"/>
    <item car:usage="voice_communication_signalling"
          car:title="@*android:string/volume_call"
          car:icon="@*android:drawable/ic_audio_ring_notif"/>
    <item car:usage="media"
          car:title="@*android:string/volume_music"
          car:icon="@*android:drawable/ic_audio_media"/>
    <item car:usage="game"
          car:title="@*android:string/volume_music"
          car:icon="@*android:drawable/ic_audio_media"/>
    <item car:usage="alarm"
          car:title="@*android:string/volume_alarm"
          car:icon="@*android:drawable/ic_audio_alarm"/>
    <item car:usage="assistance_navigation_guidance"
          car:title="@string/navi_volume_title"
          car:icon="@drawable/ic_audio_navi"/>
    <item car:usage="notification_ringtone"
          car:title="@*android:string/volume_ringtone"
          car:icon="@*android:drawable/ic_audio_ring_notif"/>
    <item car:usage="assistant"
          car:title="@*android:string/volume_unknown"
          car:icon="@*android:drawable/ic_audio_vol"/>
    <item car:usage="notification"
          car:title="@*android:string/volume_notification"
          car:icon="@*android:drawable/ic_audio_ring_notif"/>
    <item car:usage="notification_communication_request"
          car:title="@*android:string/volume_notification"
          car:icon="@*android:drawable/ic_audio_ring_notif"/>
    <item car:usage="notification_communication_instant"
          car:title="@*android:string/volume_notification"
          car:icon="@*android:drawable/ic_audio_ring_notif"/>
    <item car:usage="notification_communication_delayed"
          car:title="@*android:string/volume_notification"
          car:icon="@*android:drawable/ic_audio_ring_notif"/>
    <item car:usage="notification_event"
          car:title="@*android:string/volume_notification"
          car:icon="@*android:drawable/ic_audio_ring_notif"/>
    <item car:usage="assistance_accessibility"
          car:title="@*android:string/volume_notification"
          car:icon="@*android:drawable/ic_audio_ring_notif"/>
    <item car:usage="assistance_sonification"
          car:title="@*android:string/volume_unknown"
          car:icon="@*android:drawable/ic_audio_vol"/>
    <item car:usage="unknown"
          car:title="@*android:string/volume_unknown"
          car:icon="@*android:drawable/ic_audio_vol"/>
</carVolumeItems>
```

上面的配置和声明是在`packages/apps/Car/Settings/res/values/attrs.xml`.音量设置UI使用下面基于VolumeGroup的`CarAudioManager`APIS:

* `getVolumeGroupCount()`来知道要画多少个控制
* `getGroupMinVolume()`和`getGroupMaxVolume()`来获得高的低的边界
* `getGroupVolume()`来获得当前的音量
* `registerVolumeChangeObserver()`来获得音量改变的通知。