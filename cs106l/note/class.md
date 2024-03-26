# 类
## h和cpp
```c
#ifndef _FILENAME_H
#define _FILENAME_H
// our declarations
#endif
```


## const
`const`的对象只能调用`const`成员函数。



## template class
类模板要放在`h`中


## 重载运算符


重载类型转换运算符
```cpp
struct Foo{
  operator double()const;
};
```


尽量设计成`explicit`的，因为隐式转换会给我们带来很多意想不到的后果。


`c++20`新增了一个`<=>`运算符，返回`std::strong_order`



对象使用花括号的初始化时会调用`initializer_list`的构造方法。



