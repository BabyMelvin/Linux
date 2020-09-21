# c++11使用using定义别名(替代typedef)
typedef 普通使用
```cpp
typedef unsigned int uint_t;
```

typedef 限制 无法重定义一个模板

```cpp
typedef std::map<std::string, int> map_int_t;
typedef std::map<std::string, std::string> map_str_t;
```
需要一个固定std::string为key的map，映射到int或另一个std::string。简单通过typedef不容易做到。

`c++98/03`只能写成这样：

```cpp
template <typename Val>
struct str_map
{
    typedef std::map<std::string, Val> type;
};
// ...
str_map<int>::type map1;
```

str_map外敷类是必要的，复用某些泛型代码非常难受

```cpp
// C++ 11可以重定一个模板的语法
template <typename Val>
using str_map_t = std::map<std::string, Val>;

// ...
str_map_t<int> map1;
```

注意using覆盖了typedef的全部功能

```cpp
// 重定义unsigned int
typedef unsigned int uint_t;
using uint_t = unsigned int;

// 重定义std::map
typedef std::map<std::string, int> map_int_t;
using map_int_t = std::map<std::string, int>;

typedef void *(func_t)(int, int);
using func_t = void(*)(int, int);
```

using 重定义的 func_t 是一个模板，但它既不是类模板也不是函数模板（函数模板实例化后是一个函数），而是一种新的模板形式：**模板别名**（alias template）。
通过using可以轻松定义任意类型的模板表达方式

```cpp
template <typename T>
using type_t = T;
// ...
type_t<int> i;
```
type_t 实例化后的类型和它的模板参数类型等价。这里，type_t<int> 将等价于 int。
