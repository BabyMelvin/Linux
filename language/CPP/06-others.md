### 6 其他
（1）static 数据成员的类型可以是该成员所属的类类型。非 static 成员只能是自身类对象的指针或引用 
```c
class Screen 
	{
	public:
	         // ...
	private:
	         static Screen src1; // ok
	         Screen *src2;       // ok
	         Screen src3;        // error
	}; 
```
	
（2）非 static 数据成员不能用作默认实参，static 数据成员可用作默认实参
```c
class Screen 
	{
	public:
	          Screen& clear(char = bkground);
	private:
	         static const char bkground = '#';//static const整形变量可以在类内部初始化。
	};