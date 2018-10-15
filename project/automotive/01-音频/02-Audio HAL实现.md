汽车音频实现基于[ Android Audio HAL](https://source.android.com/devices/audio/implement)标准,包含如下：

* `IDevice`(`hardware/interfaces/audio/2.0/IDevice.hal`).创建输入和输出流，处理主音频和混合，并且使用：
	* `createAudioPatch`:创建设备支架外部外部补丁。
	* `IDevice.setAudioPortConfig()`:每个物理流提供音量。
* `IStream`(`hardware/interfaces/audio/2.0/IStream.hal`)允许带有输入和输出变量，管理实际音频采样流输入和输出硬件设备。

# 1.车辆设备类型

下面设备类型和车辆平台相关：

|Device type|description|
|--|--|
|AUDIO_DEVICE_OUT_BUS|从Android的原始输出(这个允许多有来自Android音频发送给车辆)。用作消除每个Context流的歧义地址|
|AUDIO_DEVICE_OUT_TELEPHONY_TX|用于路由到蜂窝无线电传输的音频|
|AUDIO_DEVICE_IN_BUS|用于未分类输入|
|AUDIO_DEVICE_IN_FM_TUNER|用于广播无线电输入|
|AUDIO_DEVICE_IN_TV_TUNER|用于电视设备|
|AUDIO_DEVICE_IN_LINE|用于AUX输入插孔|
|AUDIO_DEVICE_IN_BLUETOOTH_A2DP|音乐通过蓝牙接收|
|AUDIO_DEVICE_IN_TELEPHONY_RX|用于从与电话呼叫相关联的蜂窝无线电接收的音频|

# 2.路由音频源
多数音频捕获通过`AudioRecord`或者相关Android机制。捕获的数据被分配`AudioAttributes`通过`AudioTrack`要么通过默认Android路由逻辑或者显示的调用`AudioRecord`和`AudioTrack`对象上的`setPreferredDevice()`接口。

对于具有主用硬件连接到外部混合设备，或者极高延迟要求的音频源，可以使用`createAudioPatch()`和`releaseAudioPatch()`类激活或释放外部设备之间的路由。(不需要`AudioFlinger`在采样中参与)
# 3.配置音频设备
Android可使用的音频设备必须在`system/etc/audio_policy_configuration.xml`中配置，这个文件包含如下组件：

* **module name**:模块名和相应的音频驱动应该被编译到`audio.primary.$(variant).so`文件中，支持的有：
	* `primary`:用于汽车用例
	* `A2DP`
	* `remote_submix`
	* `USB`
* **devicePorts**:包含可从此模块访问的所有输入和输出设备(包含永久连接的和可移动设备)的设备描述符列表。：
	* 对所有输出设备，可以定义增益控制由`min/value/step/default`在分贝值组成(`1 millibel = 1/100 dB = 1/1000 bel`)
	* 在`devicePort`地址属性能够用来发现设备，即使有同一类型`AUDIO_DEVICE_OUT_BUS`多个设备。
* **mixPorts**:包含音频HAL公开的所有输出和输入流的列表。每个`mixPort`对`AudioService`可看做一个物理流。
* **routes**:定义输入和输出设备之间或流与设备之间可能的连接列表。

下面定一个输出设备`bus0_phone_out`的例子，这个例子汇总所有音频流都通过`mixer_bus0_phone_out`进行混合。路由输出流从`mixer_bus0_phone_out`到`device bus0_phone out`

```xml
<audioPolicyConfiguration version="1.0" xmlns:xi=/>
 <modules>
	<module name="primary" halVersion="3.0">
		<attachedDevices>
			<item>bus0_phone_out</item>
		</attachedDevices>
		<defaultOutputDevice>bus0_phone_out</defaultOutputDevice>
		<!--流的源source，列表，channelMasks过滤条件-->
		<mixPorts>
			<mixPort name="mixport_bus0_phone_out"
				role="source"
				flags="AUDIO_OUTPUT_FLAG_PRIMARY">
				<profile name="" format="AUDIO_FORMAT_PCM_16_BIT"
					samplingRates="48000"
					channelMasks="AUDIO_CHANNEL_OUT_STEREO"/>
			</mixPort>
		</mixPorts>
		<!--输入输出设备的控制address和gain控制-->
		<devicePorts>
			<!--输出设备-->
			<devicePort tagName="bus0_phone_out"
					role="sink"
					type="AUDIO_DEVICE_OUT_BUS"
					address="BUS0_PHONE">
					<profile name="" format="AUDIO_FORMAT_PCM_16_BIT"
						samplingRates="48000"
						channelMasks="AUDIO_CHANNEL_OUT_STEREO"/>
				<gains>
					<gain name="" mode="AUDIO_GAIN_MODE_JOINT"
						minValueMB="-8400"
						maxValueMB="4800"
						defaultValueMB="0"
						stepValueMB="100"
				</gains>
			</devicePort>
		</devicePorts>
		<!--路由列表-->
		<routes>
			<route type="mix" sink="bus0_phone_out"
				sources="mixport_bus0_phone_out"/>
		</routes>
	</module>
 </modules>
</audioPolicyConfiguration>
```
# 4.指定devicePorts

汽车平台应该对每个从Android输出和输入的物理流指定一个`devicePort`.比如**输出**：`devicePorts`的类型应该是`AUDIO_DEVICE_OUT_BUS`,地址应该是整数(Bus 0,Bus 1等)。`mixPort`应该和`devicePorts`创建1对1映射,并且对能够在bus路由，应该指定特定的数据格式。

车辆实现能够使用多种**输入设备类型**，包括`FM_TUNER`(给广播录音机保留使用)。`MIC`设备处理micphone输入，`TYPE_AUX_LINE`代表模拟输入设备。所有其他输入流设备都用`AUDIO_DEVICE_IN_BUS`.发现设备通过`AudioManager.getDeviceList()`枚举出来。特别来源可以通过`AudioDeviceInfo.getProductName()`进行区分。

也可以定义额外的设备作为端口，然后使用这些端口和额外的硬件交互。使用HAL`IDevice::createAudioPatch`方法。(通过一个`CarAudioManager`暴露一个入口点)

当存在基于BUS音频驱动被使用，应该设置`audioUseDynamicRouting`为`true`

```xml
<resource>
	<bool name="audioUseDynamicRouting">true</bool>
</resource>
```

具体参考：`device/generic/car/emulator/audio/overlay/packages/services/Car/service/res/values/config.xml.`
