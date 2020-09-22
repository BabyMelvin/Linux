# 初始化空指针
实际开发中，避免产生“野指针”最有效的方法，就是在定义指针的同时完成初始化操作，即便该指针的指向尚未明确，也要将其初始化为空指针。

C++98/03 标准中，将一个指针初始化为空指针的方式有 2 种：

```cpp
int *p = 0;
int *p = NULL // 推荐使用
```

C++ 中将 NULL 定义为字面常量 0，虽然能满足大部分场景的需要，但个别情况下，它会导致程序的运行和我们的预期不符。例如：

```cpp
#include <iostream>
using namespace std;

void isnull(void *c) {
    cout << "void*c" << endl;
}

void isnull(int n ) {
    cout << "int n" << endl;
}

int main() {
    isnull(0);
    isnull(NULL);

    return 0;
}
```
程序执行结果为：

```
int n
int n
```

```cpp
// 只有强制类型转换
isnull((void *)NULL);
isnull((void *)0);
```
为了修正 C++ 存在的这一 BUG，C++ 标准委员会最终决定另其炉灶，在 C++11 标准中引入一个新关键字，即 nullptr。

**nullptr**可以被隐式转换成任意的指针类型

```cpp
int *a1 = nullptr;
char *a2 = nullptr;
double *a3 = nullptr;
```

另外，通过将指针初始化为 nullptr，可以很好地解决 NULL 遗留的问题，比如：

```cpp
#include <iostream>

using namespace std;
void isnull(void *c) {
    cout << "void *c" << endl;
}

void isnull(int n) {
    cout << "int n" << endl;
}

int main() {
    isnull(NULL);
    isnull(nullptr);
    return 0;
}
```

程序执行结果为：

```
int n
void*c
```
