# 1.StringBuffer

* 如果一个字符串要经常改变，则必须使用StringBuffer。
* `str+=x；`//循环修改String，产生多个垃圾，会占用内存。

```java
public StringBuffer()
public StringBuffer append(char c)
public StringBuffer append(String str)
//查看指定字符串是否存在
public int indexOf(String str)
public StringBuffer insert(int offset,String str);
public StringBuffer reverse()
public StringBuffer replace(int start,int end,String str);
public StringBuffer delete(int start,int end);
public String substring(int start)
public String substring(int start,int ent)
public String toString()
```
# 2.Runtime类

java 中Runtime类表示实时操作类，是一个封装了JVM进程的类（**单例设计思想**），获得Runtime实例：`Runtime run=Runtime.getRuntime();`

```
//取得Runtime类实例
public static Runtime getRuntime();
//返回Java虚拟机中的空闲内存量
public long freeMemory();

//返回JVM的最大内存量
public long maxMemory()

//运行垃圾回收器，释放空间
public void gc();

//执行本机命令
public Process exec(String command) throws IOException;
```
Runtime是每一个JVM实例，取得一些系统信息。

### Runtime和Process类

```java
Runtime run=Runtime.getRuntime();
Process pro=null;
pro=run.exec("notepad.exe");
pro.destory();
```
# 3.System类
System类中的所有属性都是静态的。System类是与系统相关属性的方法集合。

```java	
//系统退出，Status非0就表示退出
public static void exit(int status)
//运行垃圾回收机制，调用的是Runtime类中的gc()方法。
public static void gc()
public static long currentTimeMillis();
//数组复制操作
public static void arraycopy(Object src,int srcPos,Object dest,int destPos,int lenght);
public static Properties getProperties()//系统所有属性
public static String getProperty(String key)
```

### 垃圾对象的回收

一个对象如果不再被任何**栈内存**所引用，那么此对象就可以被称为**垃圾对象**。等待被回收，等待时间不确定的，可以直接被调用`System.gc()`方法进行垃圾回收。

* 垃圾回收机制，对象等待回收时间是不一定的。通过`System.gc()`回收垃圾。
* 如果在对象回收之前做某些操作，Object类中有一个`finalize()`.`protected void finalize() throws Throwable`
`Throwable`JVM和程序的错误抛出

```java
class Person{
	private String name;
	private int age;
	public Person(String name,int age){
		this.name=name;
		this.age=age;
	}
	public void finalize() throws Throwable{
		System.out.println("对象被释放--->"+this);
	}
}
public class SystemDemo04{
	public static void main(String[] args){
		Person per=new Person("小李"，30);
		//没有指向栈内存
		per=null;
		System.gc();
	}
}
```

### 对象声明周期

[对象声明周期](image/05-01.png)

一个类加载后进行初始化。然后进行实例化，对象实例化会调用构造方法完成。一个对象不再使用时要等待垃圾对象收集，然后对象终结，最后被程序卸载。

# 4.日期操作类

主要是`java.util`中的Date,Calender以及`java.text`包中的SimpleDateFormat。

## 4.1 Date类
简单操作类，直接使用`java.util.Date`类构造方法进行并输出得到一个完成的日期。如`Data date=new Date()`得到当前系统日期。输出格式`Thu Jan  1 15:18:47 CST 2015`

想要按照自己的格式显示时间可以使用Calender类完成操作。

## 4.2Calender类
Calender类可以取得的时间精确到毫秒。本省是一个抽象类通过子类来实现，子类是`GregorianCalendar`

Calender类中的常量

```java
public static final int YEAR;
public static final int MONTH;
public static final int DAY_OF_MONTH;
public static final int HOUR_OF_DAY;
public static final MINUTE;
public static final int SECOND;
public static final int MILLISECOND;
```

常用方法

```java
public static Calendar getInstance();
public boolean after(Object when);
public boolean before(Object when);
public int get(int field);
```

```java
Calendar calendar=null;
//通过子类实例化
calendar=new GregorianCalendar();
calendar.get(Calendar.YEAR);
calendar.get(Calendar.MONTH);
calendar.get(Calendar.HOUR_OF_DAY);
calendar.get(Calendar.MINUTE);
```

## 4.3 DateFormat类
DateFormat类与MessageFormat类都属于Format子类，用于格式化数据。

DateFormat是一个抽象类，无法直接实例化，一些抽象类静态方法，取得本类的实例。

```java
//等到默认的对象
public static final DateFormat getInstance();
//根据Locale得到对象
public static final DateFormat getDateInstance(int style,Locale aLocale);

//得到日期时间对象
public static final DateFormat getDateTimeInstance();
//根据Locale得到日期时间对象
public static final DateFormat getDateTimeInstance(int dateStyle,int timeStyle,Locale aLocale);
```

## 4.4 SimpleDateFormat类
转换日期的格式。使用`SimpleDateFormat`

|标记|描述|
|--|--|
|y|年，年份4位数字，需要使用yyyy表示|
|M|年中月份，月份两位数字，MM|
|d|月中天数，dd表示|
|H|一天中小时数(24小时)HH|
|m|小时中分钟数，mm|
|s|分钟中秒数ss|
|S|毫秒数，SSS|

SimpleDateFormat类中常用方法

```java
//通过一个指定的模板构造对象
public SimpleDateFormat(String pattern)
//将一个包含日期的字符串Date类型
public Date parse(String source)
//将一个Date类型按照指定格式为String类型
public final String format(Date date)
```

实例
```java
String strDate="2008-10-19 10:11:30.345";
String pat1="yyyy-MM-dd HH:mm:ss:.SSS"
String pat2="yyyy年MM月dd日HH时ss秒SSS毫秒";
SimpleDateFormat sdf1=new SimpleDateFormat(pat1);
SimpleDateFormat sdf2=new SimpleDateFormat(pat2);

//将给定字符串的日期提去出来
Date d=null;
d=sdf1.parse(strDate);

//将日期变为新的格式
sdf2.format(d);
```

# 5.对象克隆技术
支持对象克隆操作，直接使用Object类中`clone()`方法即可。具体克隆方法实现子Object中。

```java
class Person implements Cloneable{
	private String name=null;
	public Person(String name){
		this.name=name;
	}
	//需要子类复写clone方法
	public Object clone throws CloneNotSupportedException{
		//具体克隆操作由父类完成
		return super.clone();
	}
}
public class CloneDemo{
	public static void main(String[] args) throws Exception{
		Person p1=new Person("张三");
		Person p2=(Person)p1.clone();
		p2.setName("李四");
		System.out.println("原始对象："+p1);
		System.out.println("克隆之后的对象："+p2);
	}
}
```

# 6.Arrays类
