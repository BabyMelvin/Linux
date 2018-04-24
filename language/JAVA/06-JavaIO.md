#Java IO
###File 类基本介绍
* 整个IO包中，唯一与文件本身有关的类就是File类。
	* public File(String pathname);
			
				public static String pathSeperator;路径分隔符 window（；）
				public static final String seperator;window(\)
				public File(String pathname)
				public boolean createNewFile() throws IOException//创建新文件
				public boolean delete()
				public boolean exists()
				pubic boolean isDirectoy()
				public long length()//文件大小
				public Strings[] list()//目录中内容，列出名称
				public File[] listFiles()//目录中内容，列出路径
				public boolean mkdir()//创建一个路径
				public boolean renameTo(File dest)
###RandomAccessFile类
* File类只是针对文件本身进行操作，而如果要对文件内容进行操作，则使用RandomAccessFile类，可以随机读取一个文件中指定位置的数据。
###字节流和字符流基本操作
* 程序中所有数据以流的额方式传输或保存。java.io中字节流、字符流两大类。
	* 字节流输出用OuterStream类，输入使用InputStream
	* 字符流输出主要是Writer类，输入主要是Reader类。
* javaIO操作相应步骤，文件操作为例。
	* 使用File类打开一个文件
	* 通过字节流或字符流的子类指定输出位置
	* 进行读/写操作
	* 关闭输入/输出
* 字节流
	* 字节流主要是以byte类型数据，以byte数组为准，主要操作是OuptStream类和InputStream类。
	* 字节输出：OutputStream(字节流中最大的父类)

				public void close() throws IOException
				public void flush() throws IOException
				public void write(byte[] b)throws IOException
				public void write(byte[] b,int off,int len) throws IOException
		* FileOutputStream 子类
		* 写入，文件不存在则会自动创建。
				
				String str="hello world";
				byte[] b=str.getBytes();
				out.write(b[i])//out.write(b);
		* 追加新内容
			
				public FileOutputStream(File file,boolean append) throws FileNotFoundException
		* append 为true，表示在文件的末尾追加内容。
	* 字节输入流InputStream
		
			public int avaliable()throws IOException//取得文件大小
			public void close()throws IOException
			public abstract int read() throws IOException
			public int read(byte[] b) throws IOException
			1.File f=new File(""+File.separator+"");
			2.InputStream input=null;
					input=new FileInputStream(f);
			3.byte b[]=new byte[1024];
				int len=input.read(b);
				input.close();
				println(new String(b,0,len));
		* 文件读到末尾了，返回-1.
* 字符流
	>在程序中一个字符等于两个字节，那么java提供Reader和Writer两个专门操作字符流的类。
	* 字符输出流Writer
		* 此类本身是一个抽象类，则必须要子类，向文件中写入内容，FileWriter的子类。
		
				public abstract void close() throws IOException
				public void write(Sring str) throws IOExcepton 
				public void write(char[] chuf) throws IOException
				public abstract void flush() throws IOException
		* public FileWriter(File file) throws  IOException
		* 整个程序与OutputStream 操作没有太大区别，不需要转换。

				public FileWriter(File file,boolean append) throws IOException
	* 字符输入流Reader
		* 需要从FileReader子类
		
				public abstract void close() throws IOException
				public int read() throws IOException	
				public int read(char[] buf) throws IOExcepton
* 字节流和字符流的区别
	* 字节流本身不会用到缓冲区（内存），是文件本身直接操作
	* 字符流使用了缓冲区，通过缓冲区再操作文件。
	* 所有的文件在硬盘或者传输都是以字节方式进行的，而字符在内存中才会形成，字节流使用广。
###ByteArrayInputStream 和ByteArrayOutputStreams内存操作
* ByteArrayInputStream
		
			public ByteArrayInputStream(byte[] buf)//内容写入内存
			public ByteArrayInputStream(byte[],buf,int offset,int length)
* ByteArrayOutputStream

			public ByteArrayOutputStream()
			public void write(int b)//内存中输出
###管道流
* 管道流主要作用是进行两个线程间通讯
	* PipeOutputStream输出流和PipeInputStream输入流
		
			public void connect(PipeInputStream snk) throws IOException
	* 验证管道流
	
			package org.caohang.demo;
			import java.io.*;
			class Send implements Runnable{
				private PipeOutputStream pos=null;
				public Send(){
					this.pos=new PipeOutputStream();
				}
				public void run(){
					String str="Hello world";
					try{
						this.pos.write(str.getBytes());
					}catch(IOException e){
						e.printStackTrace();
					}
					try{
						this.pos.close();
					}catch(IOException e){
						e.printStackTrace();
					}
				}
				public PipeOutputStream getPos(){
					return pos;
				}
			}
			class Receive implements Runnable{
				private PipedInputStream pis=null;
				public Receive(){
					this.pis=new PipedInputStream();
				}
				public void run(){
					byte b[]=new byte[1024];
					int len=0;
					try{
						len=this.pis.read(b);
					}catch(IOException e){
						e.printStackTrace();
					}
					try{
						this.pis.close();
					}catch(IOException e){
						e.printStackTrace();
					}
					System.out.println("收到："+new String(b,0,len));
				}
				public PipedInputStream getPis(){
					return pis;
				}
			}
			public class PipedDemo{
				public static void main(String args[]){
					Send s=new Send();	
					Receive r=new Receive();
					try{
						s.getPos().connect(r.getPis());
					}catch(IOException e){
						e.printStackTrace();
					}
					new Thread(s).start();
					new Thread(r).start();
				}
			}
###打印流
* 打印流是输出最方便的类。PrintStream字节打印流和PrintWriter字符打印流
* PrintStream常用的方法
	
		public PrintStream(File file) throws FileNotFoundException
		public PrintStream(OutputStream out)
		public PrintStream printf();
		public void print(boolean b);
		public void pintln(boolean b);
* 打印流输出重新装饰一下，装饰设计模式。
		
			PrintStream ps=null;
			ps=new PrintStream(new FileOutputStream(new File(""+"")));
			ps.print("hello");
			ps.println("world!!!");
			ps.print("1+1="+2);
			ps.close();
			ps.printf()//格式化输出
###System类对IO的支持
	
			public static final PrintStream out;
			public static final PrintStream in;
			public static final InputStream err;
* System.out
		
			System.out.print();
			System.out.println();
* System.err和System.out类似，但是后台的显示。
* System.in是键盘输入流，本身是InputStream类型对象。
		
			InputStream input=System.in;
			byte b[]=new byte[1024];
			System.out.println("输入内容：");
			int len=input.read(b);
			System.out.println("内容为："+new String(b,0,len));
			input.close();
	* 改进方法
		
			InputStream input=System.in;
			StringBuffer buf=new StringBuffer();
			System.out.println("输入内容：");
			int temp=0;
			while((temp=input.read())!=-1){
				char c=(char)temp;	
				if(c=='\n'){
					break;
				}
				buf.append(c);
			}
			System.out.println("内容为："+buf);
			input.close();
* 输入输出重定向
	
		public static void setOut(PrintStream out)
		public static void setErr(PrintStream err);
		public static void setIn(InputStream in)
	*	实例
		
			System.setOut(new PrintStream(new FileOutputStream(new File(""))))
			System.out.print("www.ajfda.com")输出时候，不再向屏幕
###BufferedReader类
* 用于从缓冲区中读取内容，所有输入字节数据都放在缓冲区中。
		
			public BufferReader(Reader in)//接收一个Reader类实例
			public String readLine() throws IOException //一次将缓冲区内容全部读出来
* BufferReader中定义的构造方法只能接收字符流的实例，所以必须使用字符流和字节流转换类InputStreamReader将字节流System.in变为字符流。
			BufferReader buf=null;
			buf =new BufferReader(new InputStreamReader(System.in))
			String str=buf.readLine();
	*	程序没有长度限制，可以接收中文，以上键盘输入数据的标准格式
###Scanner类
* java专门输入数据类，此类不仅可以完成输入数据操作，也可以方便数据验证。

		public Scanner(File source) throws FileNotFoundException//从文件中接收内容
		public Scanner(InputStream source)//从指定字节输入流中接收内容
		public boolean hasNext(Pattern pattern)//是否符合指定正则表达式
		public 	boolean hasNextInt()//是否是整数
		public boolean hasNextFloat();
		public String next();//接收内容
		public int nextInt()//接收数字
		public float nextFloat()
		public Scanner useDelimiter(String pattern)	
###字符编码
> System.getProperty("file.encoding");->得到系统编码
> "中国，你好！".getBytes("ISO8859-1");指定编解

* 在计算机中表示字符，人们制定了一种编码，叫ASCII码。ASCII码由一个字节中的7位(bit)表示，范围是0x00 - 0x7F 共128个字符。在此基础进行扩展。
* IOS8859-1：单字节编码，0-255字符范围，应用于英文
* GBK/GB2312:中文国标，双字节编码，表示汉字。GBK简繁，兼容GB2312简体。
* unicode:java中使用此编码方式，最标准一种编码，使用十六进制编码，但是不兼容ISO8859-1
* UTF：节约空间，兼容ISO8859-1.不定字节，一般出现中文网站。
###对象序列化
> 对象序列化是把一个对象变成二进制的数据一种方法，方便传输或存储。

* 实现序列化的方法，实现java.io.Serializable接口。是一个标识接口，表示序列化的能力。
	
		public interface Serializiable{}
* 实现对象的序列化后，可以经过二进制进行传输。但是使用对象输出流和对象输入流（ObjectOutputStream和ObjectInputStream）,其中输出成为序列化，输入流称为反序列化。
* JVM序列化版本和反序列化版本可能不兼容。引入serialVersionUID
	
		File f=new File("");
		ObjectOutputStream oos=null;
		OutputStream out=new FileOutputStream(f);
		oos=new ObjectOutputStream(out);
		oos.writeObject(new Person("",30));
		oos.close();
	*	对象属性决定对象不同，只是属性被序列化了。
	* ObjectInputStream对象反序列化。
		
			File f=new File("");
			ObjectInputStream ois=null;
			InputStream input=new FileInputStream(f);
			ois=new ObjectInputStream(input);
			Object obj=ois.readObject();
			ois.close();
			System.out.println(obj);
			姓名：张三；年龄：30
* Externalizable接口
> 被Serializible接口声明的对象内容都将被序列化，指定序列化的内容使用Externalizable接口。

			public interface Externalizable extends Serializable{
				public void writeExternal(ObjectOutput out)throws IOException;
				//指定要保存的信息，序列化调用
				public void readExternal(ObjectInput in)throws IOException,ClassNot,FoundException;
				//读取被保存信息，反序列化调用。
			}	
			public interface ObjectOutput extends DataOutput
			public interface ObjectInput extends DataInput
			//可以像DataOutputStream和DataInputStream直接输出和读取各种类型的数据.
		* 实现Externalizable序列化，此类必须存在无参数构造方法，反序列化默认调用无参构造实例化对象。
* tansient关键字
> 对象某些属性不希望被序列化，则可以使用transient关键字声明。

		private transient String name;
		读到默认:null
* 序列化一组对象
	* 对象输出只是提供了一个对象输出操作，(writeObject(Object obj)),可以使用对象数组进行操作，因为数组属于引用数据类型，直接使用Object类型进行接收。		
###本章要点

* 各类继承关系
	* OutputStream
		* FileOutputStream
		* ByteArrayOutputStream
		* PipedOutputStream
		* ObjectOutputStream
		* FilterOutputStream
				* PrintStream
				* DataOutputStream//使用与平台无关数据操作
				* DefaultOutputStream
					* ZipOutputStream
	* InputStream
		* FileInputStream
		* ByteArrayInputStream
		* PipedInputStream
		* SequenceInputStream//将两个文件内容进行合并
		* ObjectInputStream
		* FilterInputStream
			* DataInputStream
			* PushbackInputStream//将不要数据回退到数据缓冲区待重新读取
			* InflaterInputStream
				* ZipInputStream//数据过大使用压缩数据
				  * java中三种ZIP,JAR,GZIP3
	* Writer
		* PrintWriter
		* OutputStreamWriter
			* FileWriter
	* Reader
		* InputStreamReader
			* FileReader
		* BufferedReader
		* FilterReader 
			* PushbackReader

			