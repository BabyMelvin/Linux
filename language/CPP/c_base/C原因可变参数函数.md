c函数在程序中用到以下这些宏：

```c
void va_start(va_list arg_ptr,prev_param);
type va_arg(va_list arg_ptr,type);
void va_end(va_list arg_ptr);
```

* `va_list`用来保存宏va_start、va_arg和va_end所需信息的一种类型。为了访问变长参数列表中的参数，必须声明va_list类型的一个对象定义： `typedef char *  va_list;`
* `va_start`:访问变长参数列表中的参数之前使用的宏，它初始化用va_list声明的对象，初始化结果供宏va_arg和va_end使用；
* `va_arg`:展开成一个表达式的宏，该表达式具有变长参数列表中下一个参数的值和类型。每次调用va_arg都会修改用va_list声明的对象，从而使该对象指向参数列表中的下一个参数；
* `va_end`:该宏使程序能够从变长参数列表用宏va_start引用的函数中正常返回.

va在这里是variable-argument(可变参数)的意思. 

这些宏定义在`stdarg.h`中,所以用到可变参数的程序应该包含这个头文件.下面我们写一个简单的可变参数的函数,改函数至少有一个整数参数,第二个参数也是整数,是可选的.函数只是打印这两个参数的值.

```c
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/*ANSI标准形式的声明方式，括号内的省略号表示可选参数*/
int demo(char* msg,...){
	/* 定义保存函数参数的结构 */
	va_list argp;
	 /* 纪录参数个数 */  
	int argno=0;
	/* 存放取出的字符串参数 */    
	char* para;
	/* argp指向传入的第一个可选参数，    msg是确定的参数最后那个参数 */ 
	va_start(argp,msg);
	while(1){
		/*    取出当前的参数，类型为char *. */   
		para = va_arg(argp,char*);
		if(strcmp(para,"/0")==0){
			break;
		}
		printf("参数 # %d is %s\n",argno,para);
		argno++;
	}
	va_end(argp);
	return 0;
}

void main(void){
	demo("DEMO", "This", "is", "a", "demo!" ,"333333", "/0");   
}
```
从这个函数的实现可以看到,我们使用可变参数应该有以下步骤: 

* 1.首先在函数里定义一个va_list型的变量,这里是arg_ptr,这个变 
量是指向参数的指针. 
* 2.然后用va_start宏初始化变量arg_ptr,这个宏的第二个参数是第 一个可变参数的前一个参数,是一个固定的参数.
* 3.然后用va_arg返回可变的参数,并赋值给整数va_arg的第二个 参数是你要返回的参数的类型,这里是int型.  
* 4.最后用va_end宏结束可变参数的获取.然后你就可以在函数里使 用第二个参数了.如果函数有多个可变参数的,依次调用va_arg获 取各个参数. 

# 二、可变参类型陷阱
下面的**代码是错误的**，运行时得不到预期的结果：

```c
//view plaincopy to clipboardprint?
va_start(pArg,plotNo);
//TODO 类型应改为double，不支持float   
fValue=va_arg(pArg,float);
va_end(pArg);
```

下面列出`va_arg(argp, type)`宏中不支持的type：

* `char`、`signed char`、`unsigned char`
* `short`、`unsigned short`
* `signed short`、`short int`、`signed short int`、`unsigned short int`
* `float`

在C语言中，调用一个不带原型声明的函数时，调用者会对每个参数执行“默认实际参数提升(default argument promotions)”。该规则同样适用于可变参数函数——对可变长参数列表超出最后一个有类型声明的形式参数之后的每一个实际参数，也将执行上述提升工作。
提升工作如下：

* `float`类型的实际参数将提升到`double`
* `char`、`short`和相应的`signed`、`unsigned`类型的实际参数提升到`int`
* 如果`int`不能存储原值，则提升到`unsigned int`

然后，调用者将提升后的参数传递给被调用者。所以，可变参函数内是绝对无法接收到上述类型的实际参数的。

这里有一个陷阱需要避免：va_arg宏的第2个参数不能被指定为char、short或者float类型。因为char和short类型的参数会被转换为int类型，而float类型的参数会被转换为double类型 ……

例如，这样写肯定是不对的：`c = va_arg(ap,char);`因为我们无法传递一个char类型参数，如果传递了，它将会被自动转化为int类型。上面的式子应该写成：`c = va_arg(ap,int);` ` ——《C陷阱与缺陷》p164`
