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

Arrays类是数组操作类，定义在`java.util`中，主要完成实现数组元素的查找、填充、排序。

```java
//判断两个数组是否相等
public static boolean equals();

//指定内容填充到数组之中
public static void fill(int[] a,int val);
//数组排序
public static void sort(int[] a);

//对排序后的数据进行检索
public static int binarySearch(int[] a,int key);

//输出数组信息
public static String toString(int[] a);
```

# 7.比较器
## 7.1Comparable接口

Arrays类进行数组排序操作。Object数组排序，实现Comparable接口。compareTo方法只能以下3种值：

* 1:表示大于
* -1:表示下于
* 0:表示相等

成绩由高到底排序，成绩相等，则按年龄由低到高排序：

```java
class Student implements Compareable<Student>{//指定类型为学生
	private String name;
	pirvate int age;
	private float score;
	public Student(String name,int age,float score){
		this.name=name;
		this.age=age;
		this.score=score;
	}
	public String toString(){
	}
	public int compareTo(Student stu){
		if(this.score>stu.score){
			return -1;
		}else if(this.score<stu.score){
			return 1;
		}else{
			if(this.age>stu.age){	return 1;
			}else if(this.age<stu.age){
				return -1;
			}else{
				return 0;
			}
		}
		
	}
}
public class CompareableDemo{
	public static void main(String[] args){
		Student stu[]={new Student("1",20,12),new Student("2",22,1.2)};
		Arrays.sort(stu);
		for(int i=0;i<stu.length;i++){
			System.out.println(stu[i]);
		}
	}
}
```

## 7.2另一种比较器Comparator
已实现的类，然后对其增加排序操作需要Comparator。主要利用`Arrays.sort(stu,new MyCompartor());`方法。

```
//student.java
public class Student {
    private String name;
    private int age;

    public Student(String name, int age) {
        this.name = name;
        this.age = age;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getAge() {
        return age;
    }

    public void setAge(int age) {
        this.age = age;
    }

    @Override
    public String toString() {
        return "Student{" +
                "name='" + name + '\'' +
                ", age=" + age +
                '}';
    }

    @Override
    public boolean equals(Object obj) {
        //1.地址相等
        if (this == obj) {
            return true;
        }
        //2.类型不对
        if (!(obj instanceof Student)) {
            return false;
        }
        Student stu = (Student) obj;
        if (stu.name.equals(this.name) && stu.age == this.age) {
            return true;
        } else {
            return false;
        }
    }
}

//StudentComparator.java
public class StudentComparator implements Comparator<Student> {

    @Override
    public int compare(Student o1, Student o2) {
        if (o1.equals(o2)) {
            return 0;
        } else if (o1.getAge() < o2.getAge()) {
            return 1;
        } else {
            return -1;
        }
    }
}
//Demo.java
public class Demo{
	public static void main(String[] args) {
        Student student[] = {new Student("张三1", 21),
                new Student("张三2", 22), new Student("张三3", 23)};
        Arrays.sort(student, new StudentComparator());
        for (int i = 0; i < student.length; i++) {
            System.out.println(student[i]);
        }
    }
}
```

# 8.正则表达式

### 常用正则规范

|序号|规范|描述|序号|规范|描述|
|--|--|--|--|--|--|
|1|`\\`|表示反斜线(`\`)字符|9|`\w`|表示字母，数字，下划线|
|2|`\t`|表示制表符|10|`\W`|表示非字母、数字、下划线|
|3|`\n`|表示换行|11|`\s`|表示所有空白字符串(换行，空格等)|
|4|`[abc]`|字符a、b或c,只能该集合子集|12|`\S`|所有非空白字符|
|5|`[^abc]`|除了a、b、c之外的任意字符|13|`^`|行的开头|
|6|`[a-zA-Z0-9]`|表示字母，数字组成|14|`$`|行的结尾|
|7|`\d`|表示数字|15|`.`|配置换行符之外的任意字符|
|8|`\D`|表示非数字||||

### 数量表示(X表示一组规范)

|序号|规范|描述|序号|规范|描述|
|--|--|--|--|--|--|
|1|X|必须出现一次|5|X{n}|必须出现n次|
|2|X？|可以出现0或1次|6|X{n,}|必须出现n次及以上|
|3|X*|可以出现0次或多次|7|X{n,m}|必须出现`[n,m)`次|
|4|X+|可以出现1次或多次|

### 逻辑运算(X,Y表示一组规范)

|序号|规范|描述|
|--|--|--|
|1|`XY`|X规范后跟着Y规范|
|2|<code>X&#124;Y</code>|X规范或Y规范|
|3|`(X)`|作为一个捕获组规范|

## 8.1Pattern类和Matcher类
Pattern类主要是进行正则规范，Matcher类主要执行规范，验证一个字符串是否其规范。

### Pattern类的常用方法

```java
//指定正则表达式规则
public static Pattern compile(String regex);

//返回Matcher类实例
public Matcher matcher(CharSequence input);

//字符串拆分
public String[] split(CharSequence input);
```

### Matcher类常用方法

1.匹配
```java
//执行验证
public boolean matches();

//字符串替换
public String replaceAll(String replacement);
```

```java
public class RegexDemo2 {
    public static void main(String[] args) {
        String str = "1989-02-12";
		//如果使用/出现非法使用斜杠，要进行转义后使用
        String pat = "\\d{4}-\\d{2}-\\d{2}";
        Pattern p = Pattern.compile(pat);
        Matcher matcher = p.matcher(str);
        if (matcher.matches()) {
            System.out.println("日期格式合法！");
        } else {
            System.out.println("日期格式非法!");
        }
    }
}
```

2.拆分

```java
public class RegexDemo3 {
    public static void main(String[] args) {
        String str = "AAAA123BN1BB2HHH333G";
        String pat = "\\d+";
        Pattern pattern = Pattern.compile(pat);
        String[] split = pattern.split(str);
        for (int i = 0; i < split.length; i++) {
            System.out.println(split[i]);
        }
    }
}
```

3.替换

```java
public class Regex3 {
    public static void main(String args[]) {
        String str = "AAA23BBB2345BBB";
        String pat = "\\d+";
        Pattern pattern = Pattern.compile(pat);
        Matcher matcher = pattern.matcher(str);
        //替换数字
        String s = matcher.replaceAll("_");
        System.out.println(s);
    }
}
```

## 8.2 String正则表示是支持

```java
//字符串匹配
public boolean matches(String regex);
//字符串替换
public String replaceAll(String regex,String replacement);
//字符串拆分
public String[] split(String regex);
```

```java
public class StringRegex {
    public static void main(String[] args) {
        String str = "AAA2334BBB23UUU342HH";
        String afterReplace = str.replaceAll("\\d+", "_");
        boolean matcher = "1989-02-12".matches("\\d+");
        String[] split = str.split("\\d+");
        System.out.println(afterReplace);
        System.out.println(matcher);
        for (String s : split) {
            System.out.println(s);
        }
    }
}
```

注意：`|`正则表达式表示或的概念，要对其转义写成`\\|`，其中`\\`一个`\`.

```java
public class Regex5 {
    public static void main(String[] args) {
        String string = "LXH:98|MLDB:24";
        String s[] = string.split("\\|");
        System.out.println("字符串拆分：");
        for (String str : s) {
            String[] split = str.split(":");
            System.out.println(split[0] +"   "+ split[1]);
        }
    }
}
```

# 9.定时调度
Timer是一个线程设施，可以用来实现某个时间或某段时间后安排某个任务，执行一个或定期重复执行。要和TimerTask配置使用。Timer对应一个线程。

## 9.1 Timer类

```java
//创建一个计时器并启动该计时器
public Timer();

//终止计时器，放弃安排的任务。对正在执行的任务没有影响。
public int cancel();

//将所有已经取消的任务移除，释放空间
public int purge();

//安排一个任务指定的时间执行，如果超过该时间，则立即执行
public void schedule(TimerTask task,Date time);

//安排一个指定时间执行，然后固定频率(毫秒)重复执行
public void schedule(TimerTask task,Date firstTime,long period);

//延时执行并固定执行频率
public void schedule(TimerTask task,long delay,long period);

//安排延迟时间，并近似固定频率执行
public void scheduleAtFixedRate(TimerTask task,long delay,long period);
```

* `schedule()`执行时间间隔永远是固定的，之前延时情况，之后继续之前的间隔。
* `scheduleAtFixedRate()`方法可以根据出现的延时自动调整下一次间隔的执行时间。

## 9.2 TimerTask
TimerTask是一个抽象类，如果要使用该类需要建立一个类来继承此类，并实现其中的抽象方法。

```java
//终止此任务，如果该任务只执行一次并且没有执行则永远不会再执行
//如果为重复执行任务,之后不会再执行(正在执行，执行完不再执行)
public void cancel();

public void run();

//返回最近一次要执行该任务时间(如果正在执行，则返回此任务的执行安排时间)，一般在`run()`
//中调用，用来判断当前是否有足够时间来完成该任务
public long scheduledExecutionTime();
```

```java
//MyTask.java
public class MyTask extends TimerTask {
    @Override
    public void run() {
        SimpleDateFormat sdf = null;
        sdf = new SimpleDateFormat("yyyy-MM-dd:HH:mm:ss:SSS");
        System.out.println(sdf.format(new Date()));
        System.out.println(scheduledExecutionTime());
    }
}
//TimerTask.java
public class TimerTask {
    public static void main(String[] args) {
        Timer timer = new Timer();
        MyTask myTask = new MyTask();
        timer.schedule(myTask, 1000, 2000);
    }
}
```