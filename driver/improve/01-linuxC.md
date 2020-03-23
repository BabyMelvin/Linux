GNU C对标准C 进行扩展，增强C功能


关闭内建函数添加编译参数:`-ansi-pedantic`,下面特性将会报错
 
# 1.零长度和变量长度数组

```c
struct var_data {
	int len;
	char data[0];
};
```

* 没有分配内存:`sizeof(struct var_data) = sizeof(int)`
* 保存在紧接着var_data之后的内存


变长数组，用变量定义一个数组

```c
int main(int argc, char *argv[])
{
	int i, n = argc;
	double x[n];

	for (i = 0; i < n; i ++)
		x[i] = i;

	return 0;
}
```

# 2. case 范围

支持`case x...y`语法，区间[x,y]都满足

```c
switch (ch) {
case '0'...'9': c -= '0';
	break;
case 'a'...'f': c -= 'a' - 10;
	break;
case 'A'...'F': c -= 'A' - 10;
	break;
}
```

# 3.可变宏参数

```c
#define pr_debug(fmt, arg...) \
	printk(fmt, ##arg)
```

其中`##`是为了处理不带任何参数的情况

# 4. 标号元素

```c
unsigned char data[MAX] = {[0 ... MAX-1] = 0};
```

# 5.特殊属性声明

使用方法 `__attribute__`来手动检测和优化代码

* noreturn:作用于函数，表示函数从不返回,优化函数，消除警告`asmlinkage  void do_exit(long error_code) __attribute__((noreturn));`
* format作用于函数,检查参数类型，表示函数使用printf,scanf或strftime风格参数:`asmlinkage int printk(const char* fmt, ...) __attribute__((format(printf, 1, 2)));`
* unused函数或变量，消除警告
* align属性变量、结构体或者联合体，对齐方式
* packed用于最小可能对齐方式

# 6.内建函数

GNU C提供大量内建函数，与标准C库函数功能相同。

不属于库函数其他内建函数通常以`__builtin`开始

* `__builtin_return_address(LEVEL)`返回当前函数或者调用者返回地址，LEVEL表示调用栈级数
	* 0：表示当前函数返回地址
	* 1：表示调用者返回地址
* `__builtin_constant_p(EXP)`判断一个值是否为编译时常数
	* EXP是常数则返回1
	* EXP不是常数返回0

```c
// 检测第一个参数是否为编译时常数以确定采用参数版本还是非参数版本

#define test_bit(nr, addr) \
	(__builtin_constant_p(nr) ? \
		constant_test_bit((nr), (addr)) : \
		variable_test_bit((nr), (addr)))
```

* 内建函数`__builtin_expect(EXP, C)`为编译器提供分支预测，其返回值是EXP的值

```c
// likely 和 unlikely
#define likely_notrace(x)   __builtin_expect(!!(x), 1)
#define unlikely_notrace(x) __builtin_expect(!!(x), 0)
```