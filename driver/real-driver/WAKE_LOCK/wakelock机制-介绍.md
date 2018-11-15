Wake Lock是一种锁的机制, **只要有人拿着这个锁**,**系统就无法进入休眠**, 可以被用户态程序和内核获得. 这个锁可以是**有超时的**或者是**没有超时的**,

* 超时的锁会在时间过去以后自动解锁。
* 如果没有锁了或者超时了, **内核就会启动休眠的那套机制来进入休眠**.

`PowerManager.WakeLock`:有加锁和解锁两种状态，加锁的方式有两种：

*  第一种是永久的锁住，这样的锁除非显式的放开，否则是不会解锁的，所以这种锁用起来要非常的小心。
*  第二种锁是超时锁，这种锁会在锁住后一段时间解锁.

在创建了`PowerManager.WakeLock`后，有两种机制.

* 第一种是不计数锁机制
	* 前者无论`acquire()`了多少次，只要通过一次`release()`即可解锁。
* 另一种是计数锁机制。
	* 而后者正真解锁是在（ `--count == 0` ）的时候，同样当 (count == 0) 的时候才会去申请加锁。

可以通过`setReferenceCounted(boolean value)`来指定，一般默认为**计数机制**。这两种机制的区别在于.所以`PowerManager.WakeLock`的计数机制并不是正真意义上的对每次请求进行`申请/释放`每一把锁，它只是对同一把锁`被申请／释放`的次数进行了统计，然后再去操作。


源码 位置：`frameworks/base/core/java/android/os/PowerManager.java`

# 1.应用层

讲述 应用层 申请的锁怎么传到kernel下面的，来理解  整个wakelock的框架。  比如android跑起来之后在`/sys/power/wake_lock`下面的PowerManagerService 的生成过程。

Android 提供了现成`android.os.PowerManager`类 , 类中提供`newWakeLock(int flags, String tag)`方法 来取得应用层的锁,此函数的定义`frameworks/base/core/java/android/os/PowerManager.java`

```java
PowerManager pm = getSystemSevice(Context.POWER_SERVICE);
PowerManager.WakeLock wl = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK,"MyTag");
wl.acquire();  //申请锁，这里会调用PowerManagerService里面acquireWakeLock()

————————————————
 wl.release(); //释放锁，显示的释放锁，如果申请的锁不在此释放，系统就不会进入休眠。
```

# 2.framework层

`/frameworks/base/services/java/com/android/server/PowerManagerService.java`这个类是来管理 所有的应用程序 申请的wakelock。比如音视、频播放器、camera等申请的wakelock 都是通过这个类来 管理的。

```java
static final String PARTIAL_NAME ="PowerManagerService"
nativeAcquireWakeLock(PARTIAL_WAKE_LOCK_ID, PARTIAL_NAME);
```
上面 这个函数调用Power类 里面`acquireWakeLock()`，此时的`PARTIAL_NAME`作为参数传递到底层去。

```java
public static native void nativeAcquireWakeLock(int lock, String id);
```

注：在PowerManagerService 类中没有实现nativeAcquireWakeLock，其实现体在 `frameworks/base/core/jni/android_os_Power.cpp`中，所以`nativeAcquireWakeLock()`方法时会调用JNI下的实现方法。

# 3.JNI层的实现

```cpp
//路径：frameworks/base/core/jni/android_os_Power.cpp //
 static void acquireWakeLock(JNIEnv *env, jobject clazz,  jint lock, jstring idObj){
    **************
      const char *id = env->GetStringUTFChars(idObj, NULL);
      acquire_wake_lock(lock, id);
      env->ReleaseStringUTFChars(idObj, id);
}   
```
注：在`acquireWakeLock()`中调用了 路径下`hardware/libhardware_legacy/power/power.c`下面的`acquire_wake_lock(lock, id)`

# 4.Hal
在power.c下的acquire_wake_lock(lock, id)函数如下：


```java
int acquire_wake_lock(int lock, const char* id)
{   
**************
     return write(fd, id, strlen(id));
}
```
注： fd就是文件描述符，在此 表示`/sys/power/wake_lock`,id就是从`PowerManagerService`类中传下来的参数即：`PARTIAL_NAME = "PowerManagerService" `到此 就是通过 文件系统 来与kernel层 交互的地方。

PowerManager类被应用程序调用，控制电源设备状态切换:
PowerManager类对外有三个接口函数:

* 1.`void goToSleep(long time);` //强制设备进入Sleep状态
	* **Note**:在应用层调用该函数, 应用需要在源码下编译，用系统签名，否则 调用 此函数 出错；
* 2.`newWakeLock(int flags, String tag);`//取得相应层次的锁
	*  `PARTIAL_WAKE_LOCK`:保持CPU 运转，屏幕和键盘灯是关闭的。
	*  `SCREEN_DIM_WAKE_LOCK`：保持CPU 运转，允许保持屏幕显示但有可能是灰的，关闭键盘灯
	*  `SCREEN_BRIGHT_WAKE_LOCK` ：保持CPU 运转，保持屏幕高亮显示，关闭键盘灯
	* `FULL_WAKE_LOCK`：保持CPU 运转，保持屏幕高亮显示，键盘灯也保持亮度
	* `ACQUIRE_CAUSES_WAKEUP`: 一旦有请求锁时，强制打开Screen和keyboard light
	* `ON_AFTER_RELEASE`:在释放锁时reset activity timer
		* **Note**:如果申请了partial wakelock,那么即使按Power键,系统也不会进Sleep,如Music播放时,如果申请了其它的wakelocks,按Power键,系统还是会进Sleep
* `void userActivity(long when, boolean noChangeLights);`//User activity事件发生,设备会被切换到Full on的状态,同时Reset Screen off timer.


Note:

1. 在使用以上函数的应用程序中,必须在其Manifest.xml文件中加入下面的权限:

```xml
<uses-permission android:name="android.permission.WAKE_LOCK" />
<uses-permission android:name="android.permission.DEVICE_POWER" />
```

2. 所有的锁必须成对的使用, 如果申请了而没有及时释放，会造成系统故障。如申请了`partial wakelock`,而没有及时释放, 那系统就永远进不了Sleep模式.
