# typedef
typedef与`#define`相比有3个不同之处

1. 与`#define`不同，typedef给出的符号名称仅限于类型，而不是值
2. typedef的解释由**编译器**，而不是**预处理器**执行
3. 虽然它的范围有限，但在其受限范围内，typedef 比`#define` 更灵活

```c
typedef unsigned char BYTE;
BYTE x,y[10], *z;
```

typedef 与 `#define`区别

```c
typedef char * STRING
STRING name, sign;
//相当于
char *name, *sign;
```

而`#define`

```c
#define STRING char *
STRING name, sign;
//相当于
char *name,sign;
```

# 神奇的简化
```c
typedef char (*FRPTC())[5];
```
表示一个函数类型，该函数的返回值是一个指向有5个元素的char数据的指针.

# ifdef和ifndef

```c
#include <stdio.h>
#define JUST_CHECKING
#define LIMIT 4

int main(void)
{
	int i;
	int total = 0;
	for (i = 1; i <= LIMIT; i++) {
		total += 2*i*i +1;
		#ifdef JUST_CHECKING
		printf("i = %d,running total = %d\n", i, total);
		#endif
	}
	return 0;
}
```

# if和elif

`#if`很像C中的if，`#if`后面跟整型常量表达式

```c
#if SYS == 1
#include "ibm.h"
#elif SYS == 2
#include "vax.h"
#else
#include "general.h"
#endif
```

另一种方法测试是否定义：`#if defined(VAX)`代替`#ifdef VAX`

```c
#if defined(IBMPC)
#include "ibmpc.h"
#elif defined(VAX)
#include "vax.h"
#else
#iclude "general.h"
#endif
```
