#JAVA中反射机制
> 一个类，创建对象。通过一个对象找类的名称，这称为反射机制。
> 反射学习中，一切操作都将使用Object完成，类，数组的引用都可以使Object进行接收。

* Class类
	
		package org.caohang.getclassDemo;
		class X{
		}
		public class GetClassDemo{
			public static void main(String args[]){
				X x=new X();
				System.out.println(x.getClass().getName());
			}
		}
	*	org.caohang.getclassDemo.X;
	*	其中默认继承Object类，在Object类中定义：
	
			public fianl Class getClass()
			返回值的类型是一个Class类，实际上此类事Java反射源头。
	* 所有类的对象实际上都是Class类的实例
		* Object类是一切类的父类，所有类的对象实际也就是java.lang.Class类的实例，所以所有的对象都可以转变为java.lang.Class类型表示。

	* Class本身表示一个类本身，通过Class可以完整得到一个类中完整结构，包括方法定义，属性定义等。
	
			public static Class<?> forName(String className) throws ClassNotFoundException
			//传入完整“包.类”名称实例化Class对象。
			public Constructor[] getConstructors() throws SecurityException
			//得到一个类中的全部构造方法
			public Field[] getDeclaredFileds() throws SecurityException得到本类中单独定义全部属性
			public Filed[] getFields() throws SecurityException得到继承而来全部属性
			public Method[] getMehods() throws SecurityException得到全部方法
			public Method getMethod(String name,Class...parameter Types)
			public Class[] getInterfaces()得到全部接口
			public String getName()得到一个完整“包.类”名称
			public Package getPackage()得到一个包
			public Class getSuperclass()得到一个父类
			public Object newInstance()根据Class定义的类实例化对象
			public Class<?> getComponentType()返回表示数组类型Class
			public boolean isArray()判断此Class是否是一个数组
* Class类本身没有任何构造方法，使用：
	* forName()实例化对象
	* 类.class或者对象.getClass()
* Class类的使用
	* 通过Class类本身实例化其他类的对象，使用newInstance()方法，但是保证被实例化存在一个无参构造方法
	
			package org.caohang.person
			class Person{}
			Class<?> c =null;
			c=Class.forName("org.caohang.Person")
			Person per=(Person)c.newInstance()
	* 各种高级应用中，都提倡类中存在无参构造方法
		* Java程序中，反射式最为重要的操作原理，开发设计中大量反射机制，使用反射开发时一定要保留无参构造方法
	* 可以使用指定构造方法，解决没有无参构造函数也是可以的。
		* 操作步骤
			*	通过Class类中getConstructors()取得本类全部构造方法
			*	向构造方法中传递一个对象数组，里面包含构造方法中所需各个参数
			*	之后通过Constructor实例化对象。
		* Constructor常用方法
			
				public int getModifiers()得到构造方法修饰符
				public String getName()得到构造方法名称
				public Class<?>[] getPararmeterTypes()构造参数类型
				public String toString()返回此构造方法的信息
				public T newInstance(Object... initargs)
				----
				Class<?> c=null;
				c=Class.forName("org.caohang.Person");
				Person per=null;
				Constructor<?> const[]=null;
				cons=c.getConstructors();
				per=(Person) cons[0].newInstance("caohang","26")//自动拆装参数
### 反射的应用--取得类的结构
* 反射取得一个类完整结构，使用java.lang.reflect包几个类，AccessibleObject子类：
	* Constructor:类中构造方法
	* Fileld：表示类中的属性
	* Method：表示类中的方法
* 取得类实现的所有接口
		
		public Class[] getInterfaces()
	* 返回的一个Class对象数组，之后直接利用Class类中的getName()方法输出
* 取得父类

		public Class<? super T> getSuperclass()
	* 返回Class实例，可以通过getName()方法取得名称
* 换回构造方法和修饰符
	* getConstructor()用getName()输出，但是权限是数字，而不是public,需要Modifier类完成，其中的方法：
		
			public static String toString(int mod)
		* Class<?> cl=Class.forName("org.caohang.Person");
		* Constructor<?> con[]=cl.getConstructors()
		* i<con.length
		* Class<?>=con[i].getParametrTypes()
		* int mo=con[i].getModifiers()
		* Modifier.toString(mo)+""+con[].getName();
* 取得全部方法
	* Class 中getMethods()，此方法返回Method类对象数组,想进一步取得具体方法参数，抛出异常，依靠Method类。Method类中方法：
	
			public int getModifiers()取得本方法访问修饰符
			public String getName()取得方法名称
			pubic Class<?>[] getParameterTypes()取得全部参数
			public Class<?> getReturnType()得到方法换回类型
			public Class<?>[] getExceptionTypes()得到全部抛出异常
			public Object invoke(Object obj,Object... args)通过反射调用类方法
		* IDE进行程序开发，“.”提示出全部方法，是利用反射机制完成的。
* 取得全部属性
	* 取得属性两种不同的方式：
		*	得到接口和父类中公共属性：
			
				public Field[] getFields() throws SecurityException
		* 本类中的全部属性
				
				pubic Filed[] getDeclaredFileds() throws SecutityException
		* 每个Field对象表示一个属性，Field类常用的方法：
		
				public Object get(Object obj)对象属性具体内容
				public void set(Object obj,Object value)设置属性的具体内容
				public int getModifiers()得到属性修饰符
				pubilic String getName()返回属性名称
				public boolean isAccessible()判断属性是否被外部访问
				public void setAccessible(boolean flag)设置属性能否被外部访问
				public static void setAccessible(AccessibeObject[] arrary,boolean flag)
				public String toString()返回Field类的信息
* Java反射机制深入应用
	* 调用类中指定方法或指定属性，并通过反射完成数组的操作。
	* 调用setter（）和getter（）方法
		* 从面向对象部分开强调“类中属性必须封装，封装之后属性要通过setter及getter方法，反射调用方法最重要调用setter和getter方法”
		
				public class InvokeSet{
					public static void main(String args[]){
						Class<?> cl=null;
						Object obj=null;
						try{
							cl=Class.forName("");
						}catch(){}
						try{
							obj=cl.newInstance();
						}catch(){}
						setter(obj,"name","lxh",String.class);
						getter(obj,"name")
					}
					public static setter(Object obj,String att,Object value,Class<?> type){
						Method met=obj.getClass().getMethod("set"+initStr(att),type);
						met.invoke(obj,value);
					}
					public static void getter(Object obj,String att){
							Method met=obj.getClass().getMethod("get"+initStr(att))
							System.out.println(met.invoke(obj)));
					}
					public static String initStr(String old){
						String str=old.substring(0,1).toUpperCase()+old.substring(1);
						return str;
					}
				}
* 通过反射操作属性
	*	直接通过Field类属性，但是属性都是私有的，首先将Field类中设置成能被外部访问，setAccessible(true).
		
				Class<?> cl=null;
				Object obj=null;
				cl=Class.forName("org.caohang.Person");
				obj=cl.newInstance();
				Field nameField=cl.getDeclaredField("name");
				nameField.setAccessible(true)
				nameField.set(obj,"caohang");
				prinf(nameField.get(obj));
* 通过反射操作数组
		
			public Class<?> getComponentType()
		* 反射操作中使用Array表示一个数组，取得数组的操作。
		
				public static Object get(Object arry,index)根据下标取得数组内容
				pubic static Object newInstance(Class<?> componentType,int length)已有数组类型开辟新的数组对象
				public static void set(Object array,int index,Object value)	修改指定位置的内容
* 动态代理
	*	之前代理都是静态代理，每个代理只能为一个接口服务，最好一个代理完成全部的代理功能。
	* java中实现动态代理需要java.lang.reflect.InvocationHandler接口和java.lang.reflect.Proxy类支持。
		
			public interface InvocationHandler{
				public Object invoke(Object proxy,Method methmod,Object[] args) throws Throwable
			}
		* proxy：被代理的对象
		* methmod:要调用的方法
		* args[]:方法调用时所需要的参数
* 类的生命周期
	* 编译完成开始使用类，离不开JVM。
	* 程序执行中，JVM 通过装载、链接、初始化完成。
		* 装载:通过类加载器把.class二进制文件装入JVM方法区，并在堆区创建描述该类的java.lang.Class对象，用来封装数据。同一个类只加载一次。
		* 链接：把二进制数据组装成运行状态。
			* 校验：确认此二进制文件是否适合当前JVM版本。
			* 准备：为静态成员分配空间，设置成默认值
			* 解析：转换常量池的代码引用为直接引用过程。
		* 初始化：初始化之后类的对象直接使用，知道对象不再使用，被垃圾回收，释放空间。
		* 类
			* 装载、链接、出现初始化

		* 对象
			* 对象实力化、垃圾收集、对象终结、卸载。
		