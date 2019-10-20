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

