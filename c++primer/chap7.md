# 类
**定义在类内的函数是隐式的`inline`函数

### this
类内的一个指针，不可改变指向，可以通过`this->mem`来调用类内数据。


### const成员函数
`std::string isbn() const { return bookNo; }`实际上等价于
`std::string isbn(const Sales_data *const this){return this->isbn;}`

上面的例子应该还是有些模糊，当我们不希望成员函数借助`this`修改类内内容时，就需要在之后加个`const`，这样函数内隐式调用的`this`就从`Sales_data *const this`变成了`const Sales_data * const this`,因此无法借助`this`修改内容。

### 返回this对象的函数
成员函数可以返回自己的对象。
```cpp
classA &classA::foo(){
  return *this;
}


classA obj1 ;
obj1.foo().mem = 5;
```

### 构造函数
对象被初始化时就会调用构造函数。

类会有一个默认的构造函数，没有任何参数，初始化规则如下
- 类内有初始值就按初值初始化
- 否则默认初始化成员

默认构造函数只有在类没有定义构造函数时才会生成。

且如果类有其他的类作为成员，该成员又没有默认的构造函数，那么不能生成默认构造函数(套娃，成员没有默认构造，则本类也不能有)


默认构造可以显式的声明
`Person() = default;`

构造函数有一个初始值列表，可以不在函数体内给成员变量赋初值。
`Person(string n,string a):name(n),add(a){};`


### 拷贝，赋值，和析构
反正现在的都是自动处理的，不能处理的放在后面讲了
### 访问控制和封装
`public`和`private`


`class`和`struct`唯一的区别就是成员的访问权限不一样。
在第一个访问说明符之前定义的成员在`class`里是`private`，而`struct`里是`public`的.
因此我们常在类中所有成员都是`public`的时候用到`struct`.


### 友元
友元函数可以访问`private`成员
友元函数在类内的声明并非是函数声明，因此还需要在类外声明一次。
友元函数可以在类内被定义，但是必须在类外显式声明一次。

友元类也可以访问私有成员



## 类的其他特性
可以在类内定义类型，使用`using`或`typedef`，类型同样受`public` `private`限制.
这种成员被称作类型成员，必须先定义再使用，普通成员则没有这一限制。

#### 可变数据成员
有时我们也可能需要在`const`的成员函数中修改某个成员变量，可以在变量声明中加入`mutable`来实现。

可变成员永远不是`const`，即使对象是`const`


#### 基于const的重载
可以通过返回`const`的引用以及`const`成员函数来实现重载函数的效果

#### inline
**`inline`函数的定义要放在头文件中，否则可能找不到定义**


## 类的作用域
一个类就是一个作用域，因此在类外定义成员函数必须提供类名


### name lookup
类的定义分两步
1. 编译成员的声明
2. 类全部可见后才编译函数体

成员定义中的块作用域的名字查找顺序:
1. 函数内部查找
2. 类内查找
3. 成员函数定义之前的作用于查找

因此一个例如下面的代码段
```cpp
using talps = double;
class A{
  using talps = string;
  talps foo(talps);
};
talps A::foo(talps t){
  // statements
}
```
函数`foo`返回值在类内声明的时候`talps`代表的`string`,在类外定义时由于返回值不属于函数的作用域内，所以类外定义中的`talps`是`double`，而参数中的`talps`则是类内声明过的别名。

#### 类内的别名要特殊处理
一般来说内层作用域可以重新定义外层作用域中的名字,但是在类中不可以定义外层作用于定义过的名字，编译器不一定会报错


## 再谈构造
如果没有在构造函数初始化列表中初始化成员，则成员会在构造函数前被默认初始化，之后在执行函数。

### 成员初始化的顺序
构造函数的初始化列表并不限定初始化执行的顺序。
**初始化的顺序于在类中定义的顺序一致,因此最好使用参数初始化成员**



### 委托构造函数
委托构造函数可以在初始化列表中调用其他的构造函数来帮忙初始化该对象。
```cpp
class student{
  student(int a,int b,int c):a(a),b(b),c(c){}; // 常规的构造函数

  student():student(1,2,3){};
}
```

#### 默认构造函数的作用
对象在被默认初始化或值初始化时自动执行默认构造函数

默认初始化发生在
- 定义局部变量时没有赋值
- 类内有类成员，然后执行外层类的默认构造也会执行类成员的默认构造
- 类成员在构造初始化列表中没有显式初始化

值初始化发生在
- 数组初始化提供的初值小于数组大小
- 静态变量没赋初值
- 例如`vector<int> vec(10)`这种情况,显式的请求值初始化


### 隐式的类类型转换
一言以蔽之，就是利用只有一个参数的构造函数构造一个新的对象，美其名曰类型转换。
```cpp
class stu{
  stu(int a){});
}
void add(stu s1);

add(1);
```
在上述例子中，调用`add`时，隐式的调用了`stu`的构造函数构造了一个新的`stu`对象传入`add`当中。

我们可以在构造函数声明前添加`explicit`使得构造函数不会进行类型转换。
对于多个参数的构造函数由于不会进行类型转换，也就不需要`explicit`了。

不过即使`explicit`过了我们也可以通过强转来调用该构造函数实现隐式转换。
```cpp
class stu{
  explicit stu(int a){});
}
void add(stu s1);

add(static_cast<stu>(1));
```

**在类类型的隐式转换中，函数参数不能是引用，因为类类型隐式转换实质上就是利用构造函数创建了一个临时的对象传入函数，我们无法获得临时对象的引用**


### 聚合类
满足如下特点的类是聚合类
- 所有成员都是`public`
- 没有定义构造函数
- 没有类内初值
- 没有基类

这样的类我们可以用列表初始化来初始化。
```cpp
struct data{
  int a;
  string b;
};
data  temp = {1,"hello"};
```
如果列表内的元素个数少于成员，则之后的成员被值初始化.

### 字面值常量类
符合下面要求的类是字面值常量类
- 数据成员都是字面值
- 类必须有一个`constexpr`构造函数
- 数据成员如果有类内初始值，则必须是常量表达式，类内的类成员初始值必须使用自己的`constexpr`构造函数
- 类必须使用默认析构函数

**字面值常量类**没看太懂

## 类的静态成员

静态成员跟着类走，而非对象，也需要把静态成员的定义放在头中。

一般情况下类的静态成员不应在类内初始化。但是我们可以为静态成员提供`const`整数类型的类内初值，要求静态成员必须是字面值常量类型的`constexpr`.


静态成员可以作为类内的函数的默认参数，而普通的成员不行。

静态成员还是受访问权限影响的。


**静态成员只能用`constexpr`当初值，否则不能有初值**
