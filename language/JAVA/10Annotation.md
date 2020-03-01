# Annotation简介

J2SE5.0提供元数据的支持，注释（Annotation）
Annotation用来修饰类、属性、方法，不影响程序执行

```java
public interface Annotation{
	public Class<? extends Annotation> annotationType();//返回注解类型
	public boolean equals(Object obj);
	public int hashCode();
	String toString();
}
```
系统内建的Annotation

* ` @Override`:覆写Annotation, 保证方法覆写的正确性（编译进行检测）
```java
@Target(value=METHOD)
@Retention(value=SOURCES)
public @interface Override
```

* @Deprecated:不赞成使用Annotation
	* 方法和类，编译出现警告

```java
@Documented
@Retention(value=RUNTIME)
public @interface Deprecated
```
* @SuppressWarnings:压制安全警告的Annotation
	* 用来压制警告信息

```java
@Target(TYPE,FIELD,METHOD,PARAMETER,CONSTRUCTOR,LOCAL_VARIABLE})
@Retention(value=SOURCE)
public @interface SuppressWarnings
```
* SuppressWarning中的关键字
	*	deprecation(反对，贬低) 不赞成使用方法或类
	*	unchecked 执行未检查警告，泛型为指定
	*	fallthrough switch中未使用break
	*	path 设置路径出现错误
	*	serial 序列化中缺少serialVersionUID定义
	*	fianlly 任何finally子句不能完成时警告
	*	all 上所有警告

### 自定义Annotation
格式

```java
		[public] @interface Annotation名称{
			数据类型 变量名称();
		}
		单值和多值都要进行赋初值
		public @interface MyDefaultAnnotation{
			public String value();
			public String key();
		}
		多值的操作
		public @interface MyDefaultAnnotation{
			public String[] value();
		}
		默认值
		public @interface MyDefaultAnnotation{
			public String key() default "caohang"; 
		}
		枚举类型
		public enum MyName{
			MK,DK,LS;
		}
		public @interface MyDefaultAnnotation{
			public Myname name() default Myname.MK;
		}
```
**Retention** 和 **RetentionPolicy** 保留，政策
* Retention定义一个Annotation保存范围，有如下定义：

```java
@Documented
@Retention(value=ANNOTATION_TYPE)
@Target(value=ANNOTATION_TYPE)
public @interface Retention{
	RetentionPolicy value();
}
```

其中RetentionPolicy用于指定Annotation保存范围。
    * SOURCE Annotation类信息只保留在程序源文件中（`*.java`）,编译后不会保存在(`*.class`中)
	* CLASS 是编译的过程不会JVM中，没有声明默认是这个范围。
	* RUNTIME 信息保存在 `.java,.class`和JVMJ运行的时候

### 通过反射取得Annotation

想让Annotation起作用，结合反射机制。在Class类中有下列与Annotation操作有关方法。

```java
public <A extends Annotation> A 
getAnnotation(Class<A> annorationClass)//有就取得所有注释
public Annotation[] getAnnotation()返回此元素所有注释
public Annotation[] getDeclaredAnnotations()返回直接存放此元素所有注释
public boolean isAnnotation()是否有注释
public boolean isAnnotationPresent(Class<? extends Annotation> annotationClass)
```
**@Target注释**

* 没有明确指定位置，可以任意位置调用。如果想只能指定位置出现需要@Target进行注释声明。
* @Target注释中存在ElementType[]枚举类型变量，指定Annotation使用限制。

```java
public static ElementType ANNOTATION_TYPE 用在注释声明上
public static ElementType CONSTRUCTOR构造方法
	//FIELD 字段声明
	//LOCAL_VARIABLE 局部变量
	//METHOD 方法声明上
	//PACKAGE 包的声明上
	//PARAMETER 参数声明上
	//TYPE 类、接口、枚举类型上
```
**@Documented**注释
* 任何一个自定义Annotation实际通过@Document 进行注释，在生成javadoc时候可以通过@Document 将一文档说明信息写入。
* 格式：

```java
				@Document
				[public] @interface Annotation名称{
					数据类型 变量名称();
				}
```

* 使用@Document注释后，Annotation生成java doc 就可以加入类或者方法的一些信息说明，便于用户的了解类或方法的作用。
* `javadoc -d doc SimpleBeanDocumented.java`
	* 直接打开index.html 就是之前编写好的doc 文档

** @Inherited注释**
* @Inherited用于标注一个父类是否可以被子类继承，如果一个Annotation需要被子类继承，则声明直接使用@Inherited注释即可。
