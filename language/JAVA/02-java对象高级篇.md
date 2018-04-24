#面向对象
###继承
* 只允许多层继承，不能多重继承
	* java中只允许单继承，不能使用多重继承。一个子类只能继承一个父类。
	* 子类实例化的过程：
		* 子类对象实例化之前首先调用父类的构造方法，再调用子类自己的构造方法。
	* 方法覆写：
		* 子类对父类方法，方法同名。但是子类的权限不能比父类更严格。重载，发生在同一类。腹泻，发生在继承关系中。
		* this，访问本类的属性，没有从父类查找。super访问父类的属性，调用构造方法时候，需放在首行。
* Final关键字
	* 使用final声明的类不能有子类
	* 使用final声明方法不能被子类所覆写
	* 使用final声明的变量即常量，常量不可以修改
* 抽象类
	* 包含一个抽象方法的类必须是抽象类
	* 抽象类和方法都用abstract声明
	* 抽象方法只需要声明不需要实现
	* 抽象类必须被子类继承，该子类不是抽象类，必须继承所有的抽象方法（不能使用private方法）
* 接口的基本概念
	* 全局常量和公共的抽象方法所组成。

			其中不写public 也是默认全局
			interface A{
				public static final String name="caohang";
				public abstract void print();
				public abstaract String getInfo();
			}
    * 可以同时实现多个接口，但是实现所所有的抽象方法。
 ##多态
* Java中面向对象主要体现：
	* 方法的重载与覆写
	* 对象的多态性（两种类型）
		* 向上转型：子类对象->父类对象
		* 向下转型：父类对象->子类对象
	* 对于向上转型，程序自动完成。向下转型，必须声明要传递子类的类型。

			对象向上转型：父类 父类对象=子类实例
			对象向下转型：子类 子类对象=（子类）父类实例
	* class B extends A
	
			B b=new B();定义子类实例对象
			A a=b;发生向上转型关系，子类--->父类
			-----------------------------------
			A a=new B();发生向上转型关系，子类--->父类
			B b=(B)a; 此时发生向下转型。
	* 再设计的过程中，永远不要去继承一个已经实现好的类，只能继承抽象类或者实现接口。一旦发生对象向上传递关系后，方法被子类所覆写。
* instanceof关键字
	* 返回boolean类型(a2 instanceof A)
###Object 类
* java中多有的类都有一个公共的父类Object，一个子类只要没有明显继承一个父类，则肯定是Object类的子类
	*	Object中的主要方法
		
				public boolean equals(Object obj);对象的比较
				public int hashCode();取得Hash码
				public String toString();对象打印调用
	*	toString(),b和b.toString()调用的结果一样。
		*	子类中腹泻Object类中toString()方法，这样直接输出对象调用的是被子类覆写过的toString()方法.
	* equals()方法默认的是比较地址，并不能对内同进行比较。
	* 很多类库设计都采用Object作为方法的参数，这样比较方便。

###包装类
* java一切事物皆对象
* 基本工作类型
###匿名类部内
#异常基本概念

		try{}catch(){
		}catch(){
		}finally{
		}
* catch中寻找匹配的，不管程序是否产生异常，肯定会执行finally语句。没有finally,则跳到finally之后执行.
* Java中异常结构，最常用的类分别是Exception和Error,都是Throwable的子类。
	* Exception：程序中出错，可以用try....catch
	* Error:JVM错误，程序无法处理.
* throws 和 throw关键字
	* throws定义一个方法，表示此方法不处理异常，交给调用处理。
	* throw抛出一个异常，抛出异常的实例化对象。
	
			throw new Exception("自己抛出异常");
	* finally语句块作为异常处理的统一出口，避免像throw或return语句。
* Exception和RuntimeException
	* Exception->RuntimeException->IllegalArgumentExcetion->NumberFormatException
	* Exception在程序中必须使用try....catch处理（出现异常能够执行）

			public static int parseInt(String s) throws NumberFormatException
	* RuntimeException可以不适用try...catch处理，但是出现异常，则交给JVM处理。（程序中断执行）
* 自定义异常

		class MyException extends Exception{
			public MyException(String msg){
				super(msg);
			}
		}
		public class DefaultException{
			public static void main(String args[]){
				try{
					throw new MyException(“自定义异常”);
				}catch(Exception e){
					System.out.println(e);
				}
			}
		}
* 断言
		
		1.assert boolean 表达式；
		2.assert boolean 表达式：详细信息；
	*	参数 -enableassertions起作用需要javac Test.java  ,执行java -ea Test
	*	表达式1，出现java.lang.AssertionError
	*	表达式2，自己加信息，java.lang.AssertError:"自己加的信息"
		*	断言返回boolean，但是不能作为判断语句
		*	检查的功能但是不提倡使用断言。
###包和访问权限

		package 包名称.子包名称；
* javac -d . PackageDemo01.java  ;java 包名.类名;
* import 
			
			import 包名称.类名；--->手动导入包
			import 包名称.*;---->JVM自动加载所需要的类
			性能相同，不需要不导入。
* public class 和class
	* public class 文件名称与类名称一致，一个类文件中只能有一个public class，使用class 类名和文件名不一致。
	* public 还具有访问权限的问题。pubic class 可被其他文件访问。
* 静态导入
	* 一个类中所有方法都是static声明的静态方法，可用
	
			import static 包.类.*；
	* 静态导入，调用可以直接调用其中的方法，不必用,类.静态方法的形式。
* jar 命令主要参数（java archive file ）
	* C:创建新的文档
	* V：生成详细的输出信息
	* F：指定存档的文件名