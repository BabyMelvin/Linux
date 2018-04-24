#StringBuffer
* 如果一个字符串要经常改变，则必须使用StringBuffer。
* str+=x；//循环修改String，产生多个垃圾，会占用内存。

		public StringBuffer()
		public StringBuffer append(char c)
		public StringBuffer append(String str)
		public int indexOf(String str)//查字符串是否存在
		public StringBuffer insert(int offset,String str);
		public StringBuffer reverse()
		public StringBuffer replace(int start,int end,String str);
		public StringBuffer delete(int start,int end);
		public String substring(int start)
		public String substring(int start,int ent)
		public String toString()
#Runtime类
* java 中Runtime类表示裕兴时操作类，是一个封装了JVM进程的类（单例设计思想），获得Runtime实例：
		
			Runtime run=Runtime.getRuntime();
	* Runtime是每一个JVM实例，取得一些系统信息。

			public static Runtime getRuntime()
			public long freeMemory()//返回JVM中空闲内存量
			public long maxMemory()//返回JVM最大内存量
			public void gc()//运行垃圾回收器，释放空间
			public Process exec(String command) throws IOException//执行本机命令
*Runtime和Process类

			Runtime run=Runtime.getRuntime();
			Process pro=null;
			pro=run.exec("notepad.exe");
			pro.destory();
#System类
* System类中的所有属性都是静态的。
		
			public static void exit(int status)//系统退出，Status非0就表示退出
			public static void gc()//运行垃圾回收机制，调用的是Runtime类中的gc()方法。
			public static long currentTimeMillis();
			public static Properties getProperties()//系统所有属性
			public static String getProperty(String key)
	* 垃圾回收机制，对象等待回收时间是不一定的。通过System.gc()回收垃圾。
	* 如果在对象回收之前做某些操作，Object类中有一个finalize()

			protected void finalize() throws Throwable
			Throwable JVM和程序的错误抛出