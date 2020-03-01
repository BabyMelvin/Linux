> 别忘了，停下来思考。获得属于自己的东西。

# 1.java基础

linux编译的过程：`vim HelloWorld.java->javac Helloworld.java->java HelloWorld`
## 1.1.数组

## 数据类型

*  `byte`:字节数据类型 ,有符号1字节；大小范围为`-128—127`。
*  `char` 是字符数据类型 ，无符号2字节(Unicode码 ）；大小范围 是`0—65535`；char是一个16位二进制的Unicode字符，**JAVA用char来表示一个字符**。

```java
int a[]=new int[10];
int b[][]=new int[3][4];
```

* `a.length()`; `b[].length()`;
* 基本数据类型和引用数据类型
	* 类（class）
	* 接口(interface)
	* 数组（Array）

```java		
public void fun(int... arg){
	for(int x:arg){
		System.out.println(x+".");
	}
}
```
# 2.面向对象

## 2.1 `String`常用的方法

```java	
public String(char[] value);
public String(char[] value, int offset, int count);
public String(byte[] bytes);
public String(byte[] bytes, int offset, int length);
public char[] toCharArray();
public char charAt(int index);
public byte[] getBytes();
public int length();
public String[] split(String regex);
```
			
## 2.2 Static关键字

### 属性声明表示全局属性(静态属性)

* java中常用的内存区域
	* 栈内存空间：保存所有的对象名称（引用堆内存的地址）
	* 堆内存空间：保存每个对象的具体属性内容。
	* 全局数据区：保存static类型的属性
	* 全局代码区：保存所有的方法定义
* static 声明方法
	* 成为类方法**注意**
		* 非static声明的方法，可以调用static声明的属性和方法
		* static声明的方法，不能调用非static声明属性和方法
	* static 代码块
		* 静态代码块优先于主方法执行指`main()`，而在类中定义的静态代码块优先于构造快执行，并且不管有多少对象产生，静态代码块只执行一次。

```java	
public class CodeDemo {
    static {
			System.out.println("hello world");
			System.exit(1);
	}
}
```

### 代码块

```java
class Demo {
	{
		int x=30;
		System.out.println(x);
	}
	public Demo() {
		int x=40;
		System.out.println(x);
	}
}
```
* 构造块优先于构造方法执行，并且每次实例化对象，都会执行代码块中的代码，会执行多次。
* `static代码块->主方法->代码块->构造方法`
* 构造方法私有化（单例设计模式）		

```java
class Singleton {
	private static Singleton instance= new Singleton();

	private Singeton() {
	}

	public static Singleton getInstance(){
		return instance;
	}
	public void print(){
		System.out.println("Hello world!!!");
	}
}
```

* `Singleton s=Singleton.getInstance(); s.print();`
* 所有的实例的对象其实都指向同一个地址。(`window的回收站`)

### 内部类

* 内部类的好处是**可以方便的访问外部类的私有属性**。
* 使用static定义内部类
	* static声明的内部类变成了外部类，但是用**static声明的内部类不能访问非static的外部类属性**。


```java
class Outer{
	private static String info="Hello world";

	static class Inner{
		public void print() {
			System.out.println(info);
		}
	}
}
```

* 实例化`new Outer.Inner().print()`;
* 在外部访问内部类
	* 在其他类中进行调用，格式为`外部类.内部类  内部类对象=外部类实例.new 内部类()`
		* 生成class文件是以`Outer$Inner.class`文件存在。Java中只要文件存在$,程序就以“.”替换。
	* 实例化 
		* `Outer out =new Outer();`外部对象
		* `out.Inner in=out.new Inner();`
		* `in.print()`;
	* 在方法中定义内部类

```java
class Outer{
	private String info="hello world";
	public void fun(final int temp){
		class Inner{
			public void print(){
				System.out.print(info);
				System.out.print(temp);
			}
		};
		new Inner().print();
	}
}
```

* 实例化 `new Outer().fun(30);`
