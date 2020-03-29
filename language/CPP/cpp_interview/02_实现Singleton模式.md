> 设计一个类，我们只能生成该类的一个实例

写一个Singleton类型是一个很常见的面试题

# 不好的解法

## 不好的解法，只适合于单线程环境

```c
public sealed class Singleton1
{
	private Singleton1(){}
	private static Singleton1 instance = null;
	public static Singleton1 Instance
	{
		get
		{
			if(instance == null)
				instance = new Singleton1()

			return instance;
		}
	}
};
```

* 只有instan为null才创建实例避免重复创建
* 同时构造函数定义为私有函数，这样只创建一个实例
* Singleton1的静态属性Instance中

##  不好的解法，虽然多线程环境，但效率不高
解法一中，如果多线程情况下就有问题，如果两个线程同时运行判断instance是否为null语句，可能都会创建一个实例。需要添加同步锁进行操作

```cpp
public sealed class Singleton2
{
	private Singleton2()
	{
	}
	
	private static readonly object syncObj = new object();
	private static Sington2 instance = null;
	public static Singleton2 Instance
	{
		get
		{
			lock(syncObj)
			{
				if(instance == null)
					instance = new Singleton2();
			}
			return instance;
		}
	}
}
```
但是耗时操作

# 可行的解法

## 可行的解法：加同步锁前后判断实例是否已存在

```cpp
public sealed class Singleton2
{
	private Singleton2()
	{
	}
	
	private static readonly object syncObj = new object();
	private static Sington2 instance = null;
	public static Singleton2 Instance
	{
		get
		{
			if(instance == null) {
				lock(syncObj)
				{
					if(instance == null)
						instance = new Singleton2();
				}
			}
			return instance;
		}
	}
}
```