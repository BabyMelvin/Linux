代码编译到内核和编译成模块在代码中有什么区别呢？

从模块的代码中看是一样的。入口函数都是 `module_init(fun)`，但是代码中的条件编译会使宏`module_init()`在编译到内核和编译成模块的情况下替换成不同的代码。

`include/linux/init.h`中可知

```c
#ifndef MODULE
...
#define module_init(x) __initcall(x);
...
#else /* MODULE */
...
/* Each module must use one module_init(), or one no_module_init */
#define module_init(initfn) \
 static inline initcall_t __inittest(void) \
 { return initfn; } \
 int init_module(void) __attribute__((alias(#initfn)));
...
#endif
```
当代码编译成模块时，会定义 MODULE 宏，否则不会。因为在`/usr/src/linux/Makefile`中可以看到

```makefile
336 MODFLAGS = -DMODULE
337 CFLAGS_MODULE = $(MODFLAGS)
338 AFLAGS_MODULE = $(MODFLAGS)
```
这两个变量又被 export 成为全局变量。所以可以知道，在编译成模块时，会有 MODULE 这个宏。

由以下代码可以知道

```c
#define __initcall(fn) device_initcall(fn)
#define device_initcall(fn) __define_initcall("6",fn)
085 #define __define_initcall(level,fn) \
086 static initcall_t __initcall_##fn __attribute_used__ \
087 __attribute__((__section__(".initcall" level ".init"))) = fn
```
前者实际上是编译入内核中的`.initcall6.init`这个 section而在`arch/i386/kernel/vmlinux.lds.S`中可以知道：

```
083 __initcall_start = .;
084 .initcall.init : AT(ADDR(.initcall.init) - LOAD_OFFSET) {
085 *(.initcall1.init) 
086 *(.initcall2.init)
087 *(.initcall3.init)
088 *(.initcall4.init)
089 *(.initcall5.init)
090 *(.initcall6.init)
091 *(.initcall7.init)
092 }
093 __initcall_end = .;
```
arch/i386/kernel/vmlinux.lds.S

`.initcall6.init`是`.initcall.init`的一部分,执行顺序：

```
start_kernel->rest_init
```
系统启动后在 rest_init 中会创建 init 内核线程`init->do_basic_setup->do_initcalls`

do_initcalls 中会把.initcall.init 中的函数依次执行一遍

```c
for (call = __initcall_start; call < __initcall_end; call++) {
...
(*call)();
...
}
```
于是执行了`module_init(fn)`函数