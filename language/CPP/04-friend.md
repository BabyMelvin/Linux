### 1 友元类
```c
class Husband
	{
	public:
	     friend class Wife;
	private:
	     double money;//钱是老公私有的，别人不能动，但老婆除外
	};
	 
	class Wife
	{
	public:
	     void Consume(Husband& h)
	     {
	          h.money -= 10000;//老婆可以花老公的钱
	     }
	};
	 
	Husband h;
	Wife w;
	w.Consume(h);
```
### 2 使其他类的成员函数成为友元
```c
class Husband; //1.声明Husband 
	 
	class Wife //2.定义Wife类 
	{
	public:
	     void Consume(Husband& h);
	};
	 
	class Husband //3.定义Husband类
	{
	public:
	     friend void Wife::Consume(Husband& h);//声明Consume函数。
	private:
	     double money;//钱是老公私有的，别人不能动，但老婆除外
	};
	 
	void Wife::Consume(Husband& h) //4.定义Consume函数。
	{
	     h.money -= 10000;//老婆可以花老公的钱
	}
```
注意类和函数的声明和定义的顺序：

* (1)声明类Husband
* (2)定义类Wife，声明Consume函数
* (3)定义类Husband
* (4)定义Consume函数。
 
 