##一、C++类的定义
 C++中使用关键字 class 来定义类, 其基本形式如下:
```c
class 类名{
public:
	//公共的行为或属性 
private:
	//公共的行为或属性
};
```
示例:
     定义一个点(Point)类, 具有以下属性和方法:
     ■ 属性: x坐标, y坐标
     ■ 方法: 1.设置x,y的坐标值; 2.输出坐标的信息。
实现代码:
```c
class Point{
    public:
	     void setPoint(int x, int y);
	     void printPoint();
	private:
	     int xPos;
	     int yPos;
};  
```
代码说明:

* 上段代码中定义了一个名为 Point 的类, 具有两个私密属性, int型的xPos和yPos, 分别用来表示x点和y点。
* 在方法上, setPoint 用来设置属性, 也就是 xPos 和 yPos 的值; printPoint 用来输出点的信息。    

###1 数据抽象和封装
* 抽象是通过特定的实例抽取共同特征以后形成概念的过程。一个对象是现实世界中一个实体的抽象，一个类是一组对象的抽象。
* 封装是将相关的概念组成一个单元，然后通过一个名称来引用它。面向对象封装是将数据和基于数据的操作封装成一个整体对象，对数据的访问或修改只能通过对象对外提供的接口进行。
### 2 类定义
几个重要名词：
(1) 类名
     遵循一般的命名规则; 字母，数字和下划线组合，不要以数字开头。
(2) 类成员
     类可以没有成员，也可以定义多个成员。成员可以是数据、函数或类型别名。所有的成员都必须在类的内部声明。
```c
class People
	{
	};
sizeof(People) = 1; 
```
(3) 构造函数
     构造函数是一个特殊的、与类同名的成员函数，用于给每个数据成员设置适当的初始值。
(4) 成员函数
     成员函数必须在类内部声明，可以在类内部定义，也可以在类外部定义。如果在类内部定义，就默认是内联函数。
 
### 3 类定义补充
#### 3.1 可使用类型别名来简化类
  除了定义数据和函数成员之外，类还可以定义自己的局部类型名字。使用类型别名有很多好处，它让复杂的类型名字变得简单明了、易于理解和使用，还有助于程序员清楚地知道使用该类型的真实目的。
```c
class People
	{ 
	public: 
	     typedef std::string phonenum; //电话号码类型
	 
	     phonenum phonePub; //公开号码
	private:      
	     phonenum phonePri;//私人号码
	}; 
```
#### 3.2 成员函数可被重载
  可以有多个重载成员函数，个数不限。
#### 3.3 内联函数
  有三种：
（1）直接在类内部定义。
（2）在类内部声明，加上inline关键字，在类外部定义。
（3）在类内部声明，在类外部定义，同时加上inline关键字。注意：此种情况下，内联函数的定义通常应该放在类定义的同一头文件中，而不是在源文件中。这是为了保证内联函数的定义在调用该函数的每个源文件中是可见的。
#### 3.4 访问限制
`public`，`private`，`protected `为属性/方法限制的关键字。
##### 3.5 类的数据成员
类的数据成员中不能使用 auto、extern和register等进行修饰, 也不能在定义时进行初始化`如 int xPos = 0; //错;`
例外：
          静态常量整型（包括char,bool）数据成员可以直接在类的定义体中进行初始化，例如：`static const int ia= 30; `
 
### 4 类声明与类定义
#### 4.1 类声明(declare)
`class Screen;`在声明之后，定义之前，只知道Screen是一个类名，但不知道包含哪些成员。只能以有限方式使用它，不能定义该类型的对象，只能用于定义指向该类型的指针或引用，声明（不是定义）使用该类型作为形参类型或返回类型的函数。
```c
void Test1(Screen& a){};
void Test1(Screen* a){};
```

#### 4.2 类定义(define)

```c
class LinkScreen{
	public:
	          Screen window;
	          LinkScreen* next;
	          LinkScreen* prev;
}; //注意，分号不能丢
```
因为在类定义之后可以接一个对象定义列表，可类比内置类型，定义必须以分号结束：
```c
class LinkScreen{ /* ... */ };
class LinkScreen{ /* ... */ } scr1,scr2; 
```   
### 5 类对象
 定义类对象时，将为其分配存储空间。
`Sales_item item;` //编译器分配了足以容纳一个 Sales_item 对象的存储空间。item 指的就是那个存储空间。
 
### 6 隐含的 this 指针 
 成员函数具有一个附加的隐含形参，即 this指针，它由编译器隐含地定义。成员函数的函数体可以显式使用 this 指针。
#### 6.1 何时使用 this 指针

```c
class Screen 
	{
	...
	public:
	      Screen& set(char);
	};
	Screen& Screen::set(char c) 
	{
	      contents[cursor] = c;
	      return *this;
	}
```
### 7 类作用域
每个类都定义了自己的作用域和唯一的类型。类的作用域包括：类的内部（花括号之内）, 定义在类外部的成员函数的参数表(小括号之内)和函数体（花括号之内）。
```c
class Screen 
	{ 
	//类的内部
	...
	}; 
	//类的外部
	char Screen::get(index r, index c) const
	{
	     index row = r * width;      // compute the row location
	     return contents[row + c];   // offset by c to fetch specified character
	} 
```
**注意**：成员函数的返回类型不一定在类作用域中。可通过 类名`::`来判断是否是类的作用域，`::`之前不属于类的作用域，`::`之后属于类的作用域。例如
`Screen:: `之前的返回类型就不在类的作用域，`Screen:: `之后的函数名开始到函数体都是类的作用域。
```c
	class Screen 
	{ 
	public: 
	     typedef std::string::size_type index; 
	     index get_cursor() const; 
	}; 
	Screen::index Screen::get_cursor() const   //注意：index前面的Screen不能少
	{ 
	     return cursor; 
	} 
```
  该函数的返回类型是 index，这是在 `Screen `类内部定义的一个类型名。在类作用域之外使用，必须用完全限定的类型名 `Screen::index `来指定所需要的 index 是在类 Screen 中定义的名字。