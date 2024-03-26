# 模板和泛型
泛型编程也是实现多态的一种手段，虚函数实现的多态需要额外的运行时开销，而模板实现的泛型编程在编译时就可以确定类型了。


## 模板
### 函数模板
我们可以定义一个函数模板
```cpp
template <typename T>
int compare(const T&v1,const T &v2){
  if(v1>v2)return 1;
  if(v1<v2)return -1;
  return 0;
}
```
上述模板定义了一个比较函数，可以针对特定类型生成对应的版本。

定义以关键字`template`开始，后面跟模板参数列表。
模板参数列表定义了用到的类型或值，使用模板是，隐式或显式的指定模板实参，绑定到模板参数上。

当我们调用函数时，编译器会自动推断模板的实参，并绑定到模板参数上。

模板类型参数也可以当做返回值来使用
```cpp
template <typename T>T foo(T*p){
  T tmp = *p;
  retur tmp;
}
```

在模板参数中也可以使用`class`代替`typename`。


在模板参数中也可以定义非类型参数，这种参数表示一个值而非类型。当模板实例化时，非类型参数被用户或编译器推断的值来代替，值必须是常量表达式，从而允许编译器编译时实例化模板。

比如
```cpp
template<unsigned N,unsigned M>
int compare ( const char (&p)[N],const char (&q)[M]){
  return strcmp(p1,p2);
}
```
当调用`compare`的时候
`compare("hi","wwt");` 
会实例化出`int compare(const char (&)[3],const char (&)[4]);`这个版本的函数。

模板的非类型参数必须编译期可以确定的，因此必须是常量表达式。绑定到指针或引用的非类型参数的实参必须有静态的生命周期


模板也可以是声明为`inline`和`constexpr`，
`template <typename T> inline T min(const T&,const T&);`


模板编译，编译器遇到模板定义时不会生成代码，只有实例化模板才会生成代码。

模板实现和声明都在头文件里。

#### 模板的报错
模板的编译错误大多发生在实例化期间。

编译模板本身仅仅检查一些语法错误。
模板使用时，编译器会检查一些参数相关的内容。
实例化时才会发现类型相关的错误。依赖于编译器如何管理实例化，这类错误可能在链接时才报告。


### 类的模板
```cpp
template <typename T> class Blob {
public:
  typedef T value_type;
  typedef typename std::vector<T>::size_type size_type;
  Blob();
  Blob(std::initializer_list<T> il);
  size_type size() const { return data->size(); }
  bool empty() const { return data->empty(); }
  void push_back(const T &t) { data->push_back(t); }
  void push_back(T &&t) { data->push_back(std::move(t)); }
  void pop_back();
  T &back();
  T &operator[](size_type i);

private:
  std::shared_ptr<std::vector<T>> data;
  void check(size_type i, const std::string &msg) const;
};
```
#### 模板内引用模板类型
模板内使用其他的模板时，只需要用自己参数当作对应模板的实参就可以了。


#### 类模板的成员函数
内部定义的函数是`inline`的。
在模板外定义函数需要说明是模板类的成员函数。
`template <typename T> ret-type Blob<T>::method(T);`


当模板类作用域内使用自己的类型时，就不需要填写实参了。


#### 类模板和友元
当类模板中有非模板友元，则友元可以访问所以该模板实例。
如果友元自身也是模板，则类可以授权给所有友元模板实例，也可以只授权部分

一对一友好
类模板和另一个模板的友好关系最常见的就是建立对应实例之间的友好关系

在类模板前声明一下该类的友元，然后就可以在类模板内声明友元了

```cpp
template <typename T> class BlobPtr;
template <typename T> class Blob {
public:
  friend class BlobPtr<T>;
};
```


#### 通用和特定的模板友好关系
一个类可以将模板的所有实例都声明为自己的友元

```cpp
template <typename T> class Pal;
class C{
  friend class Pal<C> 
  // 用c实例化的模板类pal都是c的友元

  template <typename T>friend class Pal2;
  // pal2的所有实例都是c的友元
};

template <typename T> class C2{
  friend class Pal<T>; 
  // Pal<T>是 c2<T>的友元

  template <typename X> friend class Pal2;
  // 所有pal2的实例都是C2的友元
  
  friend class Pal3;
  // pal3是一个类，他是所有C2实例的友元
};
```

新标准中，也可以将模板类型参数设为友元

```cpp

template <typename T> class C{
  friend T
};
```


#### 模板类型别名
`typedef Blob<string> StrBlob;`给模板的实例类起了一个别名。
通过`using`我们可以给模板本身起别名
`template <typename T> using twin=pair<T,T>;`
这样的话，`twin<string>`就是`pair<string,string>`类型了

而且我们也可以在`using`时固定某个模板参数。
`template <typename T> using twin=pair<T,unsigned>;`


#### 类模板的static成员
和普通的静态成员类似，我们可以通过对象访问，也可以通过作用域运算符直接访问，当然后者需要类有实例。


### 模板参数

模板声明必须包含模板参数,不过声明中的模板参数可以和定义中不同

在模板代码中，编译器并没有模板类的定义，因此当通过作用域运算符访问成员是，无法得知要访问的成员是静态成员还是类型成员。
因此在访问类型成员的时候我们在前面加一个`typename`来标示出访问的是类型成员


默认模板实参：
我们可以给模板提供默认实参
```cpp
template <typename T,typename F = less<T>> 
int compare (const T&t1,const T&t2,F f= F()){
  if(f(v1,v2)) return -1;
  if(f(v2,v1)) return 1;
  return 0;
}
```

当实例化模板类想使用默认参数时，在尖括号中空着就可以了。


### 成员模板
无论是模板类还是普通类，都可以包含本身是模板的成员函数。
成员模板不能是虚函数。
写法和模板函数一样。
我们可以定义一个`debug`类，然后用模板函数重载括号，实现自定义的删除器

```cpp
class DebugDelete {
public:
  DebugDelete() = default;
  template <typename T> void operator()(T *p) const {
    std::cout << "delete a pointer \n";
    delete p;
  }
};
```

我们可以把我们自己的删除器用在`unique_ptr`上。


类模板也可以有成员模板，其实就是在成员中接收了其他类型的模板，比如类模板的构造函数可以接收其他类型的对象来构造本对象。

但是与一般的模板函数不同，当我们在类模板中定义了成员模板时，在类外定义的时候就需要顺序的声明类和成员的模板参数.
```cpp
template <typename T> class Base {
  template<typename I> Base(I);
};
// 外部实现的时候需要声明类和成员各自的模板参数
template <typename T>
template <typename I>
Base<T>::Base(I){

}
```

### 控制实例化
模板被使用时才会实例化的特性亦为之，相同的实例可能出现在多个对象文件中。即，独立的源文件都需要各自实例化模板，这个开销可能会很大。

因此我们使用了类似全局变量的方式来避免这样的问题，`extern template class B<int>`使用这样的声明来表示在其他文件中有实例化的代码,实例化代码类似`template class B<int>;`

当我们声明了`extern`的模板声明时，对应的模板类实例将不会出现在本文件编译生成的`obj`文件内。

一个类模板的实例化定义会实例化该模板的所有成员,这点与普通的而实例化模板对象不同，因为编译器无法确定实例化定义里的哪些成员不会被使用，因此干脆都实例化.


### 效率和灵活
因为`shared_ptr`的删除器并不像`unique_ptr`那样属于模板实参，运行时才会知道自己的删除器类型，我们还可以通过`reset`赋予给指针其他删除器，因此`shared_ptr`肯定不是直接保存的删除器。

而`unique_ptr`内部则存放了删除器。


## 模板实参推断
对于模板函数来说，编译器通常不对实参进行类型转换，而是根据实参生成新的模板实例。

除了`const`转换(实参无`const`可以传入形参有`const`的函数)和数组到指针的转换，其他的转换(子类对象到父类对象，类自定义的转换，以及算术类型转换)都不适用于函数模板。

**函数参数如果是引用，那么传入的数组会得到数组类型，数组不再退化成指针**

函数模板中的参数也可以有非模板类型参数的类型
`template <typename T> void foo(ostream &os,T &);`
在调用模板函数传入这些参数的时候，则可以进行参数的转换



### 显式推断实参
`template <typename t1,typename t2, typename t3> 
T1 sum(T2,T3)`
类似上面的模板函数实例化时，需要用户指定实参类型

在调用时这样调用`sum<string>(i1,i2);`

尖括号之间将返回值类型围起来。

也可以`sum<string,long,long>(i1,i2);`指定出三个类型实参。

当我们用尖括号显式的指定类型之后，正常的类型转换是可以用于模板函数的参数的。

当模板函数允许没有参数的调用，而返回值是模板类型参数时，返回值的类型就需要实例化模板时显式的用`<T>`指出


### 尾置返回类型
如果我们的模板函数接受一对儿迭代器，返回其中的一个元素引用，那么返回值是无法确定的，此时我们需要尾置返回值然后使用`decltype`来获取元素的类型

```cpp
template <typename It>
auto fcn(It beg,It end)->decltype(*beg){
  return *beg;
}
```

**`decltype`对左值返回类型的引用，右值返回类型**


类似的，我们对模板的参数类型了解的很有限，比如上述的迭代器参数，我们只能获得元素的左值引用，每个迭代器操作都会生成引用，如果我们想要返回的是值而非引用时，会很难办。

为了获取元素类型，我们可以使用标准库的类型转换模板，之后会深入介绍。

可以使用`remove_reference`来获取元素的类型,其中有个`type`成员类型，我们利用这个成员来获取元素的类型。

```cpp
template <typename It>
auto fcn2(It beg,It end)->
  typename remove_reference<decltype(*beg)>::type
{
  return *beg;
}
```


### 函数指针和实参推断
当我们将模板函数赋值给函数指针时，会根据指针类型确定模板实参。


当模板函数的参数是左值的时候，实参也必须是一个左值，
`template <typename T> void f1(T &)`
上述的模板函数也可以传入`const`的左值，这样`const`也会成为`T`的一部分。


`template <typename T> void f1(const T &)`
当模板函数的参数是`const` 时，我们可以传入任何类类型，字面值可以转换成`const`引用



引用折叠：
当我们将左值当作参数传入一个需要右值引用的函数时，会发生引用折叠。

`c++`有两个例外规则规定了引用折叠，事实上`move`也是依靠这个才能正常工作的。


当我们传入左值给需要右值引用的模板函数时，`T`会变成**实参类型的引用** 这个类型，比如
`template <typename T> void foo(T&&t){}
int i=5;
foo(i);`
这里的`foo`实际上接受的参数是`int& &&`也就是`int`引用的右值引用。


这是第一个规则。第二个规则规定了创建了引用的引用后会发生引用折叠，上述例子中的`int& &&`就折叠成了`int&`，而`T`就变成了`int&`.

引用折叠
`int& &&` `int&& &` 和`int& &`都会折叠成`int &`
`int&& &&`折叠成`int&&`
引用折叠只能应用于间接创建引用的引用，比如通过类型别名和模板参数创建的引用的引用。


这两条规则导致我们可以给需要右值引用的模板参数传入任意类型的实参，左值右值都可以。

模板参数可以推断成引用类型，可能会对我们的预期结果产生影响
```cpp
template <typename T> void f3(T &&val){
  T t= val ;// T是引用类型，还是普通的类型？
  t = fcn(t); // 还是由于T的类型导致的结果
  if(t == val)// 如果T是引用，那么一直为true
}

```

为了避免这样的事情发生，我们一般会写两个函数模板，来进行重载
`template <typename T>void f(T&&); //接收非const 右值
template <typename T>void f(const T&); // 接收左值和const右值`


### move的定义
```cpp
template <typename T> 
typename remove_reference<T>::type&& move(T&& t)
{
  return static_cast<typename remove_reference<T>::type&&>(t);
}
```
`move`的参数是右值引用，不过由于引用折叠，使得他可以接受任何类型的参数,因为`T`可以是任何有无`const`的类型。


### 转发
某些函数需要将实参连同类型不变的转发给其他函数，此时我们需要保留实参的所有性质，包括`const`属性和 以及实参的左值右值。

```cpp
template <typename F,typename T1, typename T2>
void flip1(F f,T t1,T t2){
  f(t2,t1);
}
void f(int v1,int &v2){
  v2+=1;
}

f(42,i); //i可以被改变
flip1(f,i,42); // 类型推导为int,不是引用，因此传入到内部的f时传入的是值，没有对i进行修改
```

由于上述这种情况，我们需要保留实参的属性，因此，我们可以使用右值引用。
```cpp

template <typename F,typename T1, typename T2>
void flip2(F f,T&& t1,T&& t2){
  f(t2,t1);
}
void f(int v1,int &v2){
  v2+=1;
}
```
此时`t1`如果是左值，那么引用属性会被保留，如果是右值，右值的属性也会保留，同时`const`属性会被`T`接收。


但是当我们用修改过的`flip2`调用`g`函数时
```cpp
void g(int &&i,int &j){
  
}
template <typename F,typename T1, typename T2>
void flip2(F f,T&& t1,T&& t2){
  g(t1,t2);
}
flip2(g,j,42); //发生错误
```

我们会发现右值引用的属性即使接收了也无法向其他函数传递，
因为右值引用传入后，已经是左值了(可以这么理解，右值仅仅是为了重载函数，实现移动的一个标志)。

### forward转发

```cpp
template <typename T> inter(T &&arg){
  final(std::forward<T>(arg));
}
```

通过`forward`转发，我们可以实现保留引用信息的传递参数。


## 模板的重载

函数模板会被其他的函数模板和普通的函数重载。
影响函数匹配的规则有
- 候选函数为实参推断成功的函数
- 候选的函数模板都是可行的，实参推断阶段就将不可行的模板排除了
- 如果有最好匹配的函数选择该函数，否则
  - 同样好的函数中只有一个是非模板的，则选择他
  - 都是模板，但是有一个更特例化，则选择他
  - 有歧义，报错


```cpp
template<typename T> string debug_rep(const T &t);
template<typename T> string debug_rep(T *t);

string s("wwt");
debug_rep(s); // 调用第一个参数为 引用类型的模板函数
debug_rep(&s); // 调用第二个，因为精确匹配
```

当我们调用`debug_rep(&s)`时，候选函数有两个
1. `debug_rep(const string* &)` ,第一个模板生成的
2. `debug_rep(string*)` 第二各模板生成的

事实上编译器也确实调用的是第二个，因为调用第一个需要进行一个指针到`const`指针的转换。
不过我们也由此可知模板的参数类型实际上可以接受`const` 还有指针`*`以及`&`.


看看下面的代码
```cpp
const string *sp = &s;
debug_rep(sp);
```

当`const`之后两个版本都差不多了，第一个版本`T`被绑定到了`string *`上，第二个被绑定到了`string`上，且都不需要转换。
此时编译器还是会选择第二个进行调用，因为第一个版本的模板可以接受非指针类型的参数，而第一个模板是更特例化的，也就是适用范围更窄的。


### c风格字符串和字符串字面值
数组到指针的转换被函数匹配认为是精确的转换
所以`c`风格字符串作为参数传入，会匹配数组的引用和指针，指针会更特例化所以编译器选择指针版本的。


## 可变参数的模板
可变数目的参数被放到了参数包中。
```cpp
template <typename T,typename ... Args>
void foo(const T &t,const Args &...rest);
int i=0;
double d = 3.14;
string s = "wwt";
foo(i,s,42,d);
foo(d,s);
```
根据上述的`foo`调用，会生成不同版本的函数。

可以通过`sizeof...(参数包)`来获取参数包内的参数个数。

如果参数是相同类型的，我们可以借助`initializer_list`来传递。但参数类型和个数都不同，我们就只能使用可变参数了。


可变参数函数通常都是递归的
```cpp
template <typename T>
ostream &print(ostream &os,const T &t){
 return os << t;
}
template <typename T,typename... Args>
ostream &print(ostream &os,const T &t,const Args&... rest){
  ostream << t<<",";
  return print(os,rest...);
}
```


### 包扩展
前面的代码每次调用将包内的参数拿出来一个匹配`T`类型其实就是包扩展。
包扩展为其构成元素是一种方式，还有其他的扩展模式

```cpp
template <typename... Args>
ostream &errorMsg(ostream &os,const Args&... rest){
  return print(os,debug_rep(rest)...);
}
```
上述的包扩展方式将包展开，对每一个元素执行了`debug_rep`函数，然后结果放在参数列表上然后调用`print`。

### 转发包
新标准下可以利用可变参数和`forward`的来编写函数，将实参不变的传递给其他函数

```cpp
template <typename... Args>
inline 
void alloc(allocator &alloc,int curr,Args&&... args){
  alloc.construct(curr,std::forward<Args>(args)...);
}
```
上述代码中`forward<Args>(args)...`将包展开，对每一个元素进行了转发操作。


### 模板特例化
模板特例化就是让模板的适用范围窄化，针对特定的情况进行调用。
一个特例化版本就是模板的一个独立定义。

```cpp
template <>
int compare(const char *const &p1,const char *const &p2){
  return strcmp(p1,p2);
}
```
上述就是一个模板的特例化版本，针对`compare`函数，我们特例化了一个专门用于`c`风格字符串的比较函数。

在特例化中，我们针对`T`进行特化，明确指出`T`的类型，空尖括号的含义是我们为原模板中的所有模板参数提供了实参。


一个特例化的函数本质上是一个实例，而非重载，因此这个工作是我们替编译器做了的。

模板和特例化版本应该声明在同一个头文件中，模板声明在前，特例化版本在后。

我们也可以对类模板进行特例化。
```cpp
template <>
class Base<int*>{
  // 内部实现
}
```
将`T`绑定到了`int*`之上，以后`Base<int*>`优先调用这个模板。

部分特例化，实际上叫偏特例化好理解一点
```cpp
template <typename T>
class Base<T *>{

}
```
当使用指针，左值引用，右值引用时，会优先选择对应版本的特例化模板。


只特例化成员:
```cpp
template <typename T>struct Foo{
 void Bar(){}
}

template <>
void Foo<int>::bar(){
  //   针对int的特殊处理
}
```

上述代码仅仅特例化了`bar`一个成员函数，当使用`int`实例化模板的时候，其他的成员正常实例化，`bar`成员使用我们特例化的版本。













