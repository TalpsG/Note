# 模板
## 模板函数


`SFINAE`
`Substitution Failuer Is Not An Error`

在模板函数中如果替换推导类型后失败(类型没有函数体内所需要的成员),并不会造成编译错误，只是本函数模板实例不能作为可行函数。


如果我们的函数模板需要参数有`size`这个成员。
```cpp
template <typename T>
auto printSize(const T& t)->decltype(a.size()){
  return a.size();
}
```

我们可以使用`enable_if<Predicate>`来判断，如果`Predicate`满足，则对函数模板的处理照常进行，否则会发生模板错误。



## 泛型算法
`copy_if(begin, end ,newbegin,Predicate)`
注意`newbegin`所指的位置要有足够的空间存放拷贝来的数据，我们可以使用`inserter`来生成插入器，这样即使空间不够，也可以自动进行扩容。


删除算法
`v.erase(begin,end);`会删除迭代器之间的元素
`remove_if(begin,end,pred)`会将删除元素都移动到后面,元素间的顺序是稳定的
返回一个迭代器，这个迭代器开始之后的元素都符合删除的条件。


## concepts
1. `concepts`可以限定模板类型参数的条件，报错会变得可读
2. 不同的`concepts`可以重载模板函数
