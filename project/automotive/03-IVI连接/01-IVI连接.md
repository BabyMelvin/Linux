Android 8.0使用无缝蓝牙用户体验，当连接到车辆上时。IVI监听事件，如开车门或者启动发动机，并且自动搜索附近蓝牙设备。也可以异步连接独立的设备，所以用户对任何设备很方便。

# 1.蓝牙连接管理
7.x及之前，IVI蓝牙栈只有当蓝牙适配器使能才会扫描设备。如果他们设备进入范围，IVI蓝牙开启，用户必须手动连接。Android 8.0减少IVI设置app的手动的连接步骤。

当蓝牙适配器开启并且没有连接到设备，定制触发事件(如开门或者启动引擎)让IVI蓝牙搜索范围内设备来和相应配置进行配对。IVI蓝牙使用一个优先级列表来决定哪个设备来进行连接。

除了触发事件，可以对每个配置文件具体设备的优先级。默认实现，IVI蓝牙最近连接的优先连接。有一个单独优先连接列表，所以每个服务配置能够连接不同的蓝牙设备。每个蓝牙服务配置也是有不同的连接的数量限制的

|Service|Profile|Connection limit|
|--|--|--|
|Phone|HFP|2|
|. |PBAP|2|
|Message|MAP|1|
|Audio|A2DP|1|

## 1.1实现连接管理
为了支持Android 8.0自动连接，蓝牙连接管理逻辑从Bluetooth Service移动到一个policy在Car Service中。在这个策略中，你能够定制，当Bluetooth自动连接应该扫描新设备，并且哪个设备应该首先进行连接。

默认的连接策略:`service/src/com/android/car/BluetoothDeviceConnectionPolicy.java`为了使用一个定制phone policy，**关闭默认**phone策略`res/values/config.xml`设置`enable_phone_policy`为false

为了创建一个触发事件，在Car Service注册一个相应的监听.例如一个新的传感器事件:

```java
  /**
  * Handles events coming in from the {@link CarSensorService}
  * Upon receiving the event that is of interest, initiate a connection attempt by
  * calling the policy {@link BluetoothDeviceConnectionPolicy}
  */
  private class CarSensorEventListener extends ICarSensorEventListener.Stub {
    @Override
    public void onSensorChanged(List<CarSensorEvent> events) throws RemoteException {
      if (events != null & !events.isEmpty()) {
        CarSensorEvent event = events.get(0);
        if (event.sensorType == CarSensorManager.SOME_NEW_SENSOR_EVENT) {
          initiateConnection();
        }
      ...
  }
```

为了每个配置文件配置设备优先级，修改下面的方法`BluetoothDeviceConnectionPolicy`:

* `updateDeviceConnectionStatus()`
* `findDeviceToConnect()`

## 1.2 验证连接管理
通过你的汽车IVI开关蓝牙开关管理 验证蓝牙连接的行为并且能够自动连接到合适的设备上。

能够使用`abd`命令来测试IVI模拟车辆事件：

* 对`CarCabinManager.ID_DOOR_LOCK`:`adb shell dumpsys activity service com.android.car inject-event zoned-boolean 0x16200b02 1 false`
* 对`CarSensorManager.SENSOR_TYPE_IGNITION_STATE`:`adb shell dumpsys activity service com.android.car inject-event global-integer 0x11400409 5`

# 2.蓝牙多设备连接
当有触发事件，[ Bluetooth Connection Management Policy](https://android.googlesource.com/platform/packages/services/Car/+/master/service/src/com/android/car/BluetoothDeviceConnectionPolicy.java)决定哪个设备连接到各自蓝牙文件。Policy发送一个连接意图给client服务。反过来，配置文件client服务创建一个client状态机实例来管理连接的每个设备。

## 2.1