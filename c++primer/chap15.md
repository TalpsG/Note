# 面向对象
`OOP`核心思想: 数据抽象，继承和动态绑定

## 概述
### 继承
对于某些成员函数，需要派生类修改实现的要定义为`virtual`。
没有`virtual`的函数就会被不作修改的继承。


#### 动态绑定
使用基类的引用或指针调用虚函数时将发生动态绑定。

## 定义基类和派生类
在继承关系的根节点中通常都会定义一个虚析构函数

虚函数的解析过程发生在运行时。

派生类可以继承基类中的成员，但是不一定有权限访问基类中的成员。

`protected`描述的成员可以被子类成员函数访问，`private`不行。

子类不总是覆盖他继承的虚函数，如果虚函数没有被继承，那么子类会直接继承基类的版本

可以在`const`或`&`之后加一个`override`显式标注出成员函数覆盖了他继承的虚函数。


我们可以把子类对象当作基类对象使用，也可以把基类指针指向子类，基类引用绑定到子类。

### 派生类的构造函数
在子类的构造函数中，要在初始化列表处添加基类的构造函数，以便初始化基类的成员，否则将默认初始化基类成员。
初始化顺序是先基类的成员，然后按声明顺序初始化子类成员。


### 静态成员

静态成员不被继承，只有一个但都可以通过子类或基类访问。


### 类的声明
类声明时不需要写出基类。

当类继承自基类时，基类定义必须在子类定义前，不能只有基类的声明


### 类的声明
类声明时不需要写出基类。

当类继承自基类时，基类定义必须在子类定义前，不能只有基类的声明.

有时我们不希望类被继承，可以使用`final`修饰类，防止其他类继承该类
`class T final{};`


### 类的转换和继承
通常情况下，引用和指针绑定到对象上需要类型一样，但是类是一个例外，基类指针和引用可以绑定到子类上。

静态类型是代码编写时的类型，动态类型时代码运行时在内存中的对象的类型。

基类指针指向子类对象就是一个例子，动态类型运行时才可知。

#### 基类对象不能绑定到子类的指针或引用
因为不知道基类对象运行时的类型。
而且即使是如下的代码也不能进行转换
```cpp
Bulk_quote bulk;
Quote *p = &bulk; // 合法
Bulk_quote *p2 = p; // 非法
```
编译器进行静态检查时无法确定转换是否合法，因此就都当成不合法处理了。

如果基类有虚函数的话，我们可以通过`dynamic_cast`来请求类型转换，这个类型转换在运行时执行，可以检查类型转换是否安全。

如果已知类型转换是安全的，可以用`static_cast`覆盖掉编译器的检查。

类的类型转换只存在于指针和引用中，对象本身不存在类型转换。由于构造函数不是`virtual`的，因此自己的构造函数智能处理自己的成员。

```cpp
Bulk_quote bulk;
Quote item(bulk); // 会调用Quote(const Quote &) 构造函数
item = bulk;  // 同理调用Quote::operator=(const Quote&)
```

上述过程基类构造函数只能处理基类的内容，子类的内容被切掉了


**子类向父类转换时可能会受到访问限制而不可行**

## 虚函数
动态绑定是指基类的引用或指针调用虚函数时发生的操作，我们在运行时才能知道到底调用了哪个版本的虚函数。

当我们通过对象而非指针或引用调用虚函数时，并不会发生动态绑定，编译时即可确定调用版本。

我们可以显式的写出`override`来表明我们的函数重写了基类的虚函数,因为我们可以编写**参数列表不同的同名函数**，注意是参数列表不同，返回值相同和不相同无所谓，这样的函数没有覆盖`virtual`函数。
但是大多数这种情况都是写错了，如果我们没写`overried`，编译器检查不出来这种错误。

我们也可以对函数使用`final`修饰，这样可以让子类无法重写该函数

**`virtual`函数在子类的 子类也还是虚函数，因此子类重写时可以写上`virtual`，不写也成**

`final`和`override`在`const`和引用修饰符以及尾置返回类型之后。


虚函数可以有默认实参，但是由于默认实参的实现是放在编译期的，因此默认实参的类型是跟着指针引用或类对象类型走的，即**即使基类指针指向子类对象，通过指针执行虚函数的默认实参仍然是基类的默认实参，如果指针是子类对象的，则默认实参是子类的**

因此如果虚函数有默认实参，基类和子类的默认值最好一样。

### 避免虚函数的机制
子类对象可以通过作用域运算符来实现强制执行父类版本的虚函数，这样避免了动态绑定。
`son.Father::test();`
这样的函数调用在编译期就完成了解析。

一般是在子类的虚函数中要调用本函数覆盖了的父类虚函数时，会使用该方法回避虚函数的机制，这种情况下，父类的虚函数通常执行了所有继承过他的类都要完成的操作。

一般只有成员函数或者友元函数才需要来避免虚函数机制。


## 抽象基类
纯虚函数，类内没有实现的函数，告知编译器这个函数不需要定义，在声明时后面添加`=0`即可。我们也可以提供纯虚函数的定义，不过只能定义在类外。
纯虚函数只能出现在类内部的而虚函数声明语句处。


我们不能定义含有纯虚函数的类对象，但是这个类可以被其他子类继承，实现其中的纯虚函数，完成相应的语义。k

### 抽象基类
含有纯虚函数的类是抽象基类
抽象基类负责定义接口，子类继承后负责覆盖接口。
我们不能直接创建一个抽象基类的对象。
如果子类继承后仍然没有实现纯虚函数，则子类仍然为抽象基类。


## 继承和访问控制
一个类可以使用`protected`来声明那些能被子类和友元访问到的成员，`protected`类似`public`和`private`的结合。
- 和私有成员类似，类的用户无法访问,也就是无法通过对象引用或指针直接访问
- 子类成员和友元可以访问
- 子类成员只能通过子类对象来访问基类的`protected`成员，子类对于基类的受保护成员没有访问特权。

最后一条意思类似如下
```cpp
class Base{
protected:
  int prot;
}
class Son:public Base{
  friend void clobber(Sneaky &); 
  // 子类成员函数内可以借由 子类对象 对象访问基类受保护成员

  friend void clobber(Base &);
  // 子类成员函数内不能通过基类对象访问基类受保护成员
}
```
总而言之，子类的成员和子类的友元可以通过子类类型的对象的指针或引用访问子类成员访问基类的受保护内容，不能通过基类对象的指针或引用访问。

因为如果可以通过基类对象的指针或引用访问`protected`内容，则该内容实际上就没有被`protected`，因此不能这样操作。

继承时的派生权限说明符对于子类及其友元访问基类的权限没有影响。


派生访问说明符的目的是控制子类及其子类用户的子类对于基类的访问权限。
换句话说，子类的成员函数及其友元可以访问受保护的成员，但是用户不能通过子类对象直接访问受保护的成员。


当继承是`public`时，用户(也就是通过对象直接访问)对于父类成员的访问权限不变，子类对象可以访问公开和受保护的成员，私有的不行。

继承是`private`的时候，所有继承来的成员都是私有的

继承时`protected`时，继承来的共有成员都是受保护的，私有还是私有。

### 派生类向基类转换时的访问型
- 当子类共有继承基类的时候，用户代码可以使用派生类向基类转换，如果继承是私有的和受保护的，则不能转换
- 不论子类怎么继承基类，子类的成员函数和友元都能使用子类到基类的转换(应该是用在成员函数内部，可以将基类指针指向子类)
- 只有子类继承基类是公共的或受保护的，子类成员和友元才可以使用子类到基类的类型转换，私有继承不能使用


### 友元和继承
友元关系不能转递也不能继承，基类的友元对子类的访问没有特权。类似的，子类的友元对基类的访问也没有特权。

特别的，基类的友元可以访问子类中的基类成员。
友元关系不是继承的，而是由每个类控制自己成员的访问权限实现的。

对于友元类来说，友元子类和基类不具有特殊的访问能力。

改变个别成员的可访问性
```cpp
class Base{
private:
  void test();
};
class Derived:private Base{
public:
  using Base::test();
};
```
上述代码使用`using`改变了基类的私有成员的可访问性，之后继承`Derive`的子类将会按照`public`的权限使用该成员。

只有子类可以访问到的基类或者间接基类的成员才可以用`using` 改变其权限。



`struct`类继承不写继承访问符则默认是`public`继承，`class`则是`private`.


只有派生类`public`继承的时候才能进行基类指针引用到子类的转换.

**说实话，这一小节我自己确实没弄明白，记录的有些矛盾的地方**


## 继承中的类作用域

每个类定义自己的作用域，子类作用域在基类作用域内。

静态类型决定调用的成员，因此，通过基类指针引用对象调用不了子类的特有的成员。

子类的同名成员会隐藏掉父类的同名成员，要想调用父类的同名成员需要加上作用域运算符。

### 名字查找的过程
1. 确定调用方的静态类型
2. 如果对象类型中能找到调用的成员则进入下一步，否则向基类继续找，找到顶也找不到就报错
3. 如果是函数，检查参数类型
4. 如果函数是虚函数且是通过引用和指针调用的，则运行时确定调用的版本，否则编译期确定。

因此如果参数列表不同的函数是会隐藏同名的基类虚函数的。


### 覆盖重载函数
如果子类期望父类的所有的重载版本都可以被访问，则需要覆盖所有版本或者一个都不覆盖。
我们可以为重载成员提供一条`using`声明，只指定名字而不指定参数列表，这样就会把所有重载函数添加到子类中，此时我们可以去覆盖特定的函数。

对子类中没有重新定义的函数的访问实际上是访问的继承而来的`using`声明过得父类中的版本。


## 构造函数和拷贝控制
我们需要在基类中定义一个虚析构函数，这样在继承关系中就可以动态地分配内存了。
虚析构是虚函数，因此也会在运行时确定调用的版本。

虚析构的定义会导致不会合成移动操作。


### 合成拷贝控制和继承
由于类的构造函数只负责本类的内容的初始化，因此在类构造函数中会调用基类的构造函数。
拷贝构造函数也类似。

析构函数与构造相反，销毁本类的成员后继续向上销毁父类。

由于基类定义了析构，导致了没有合成的移动操作，也会导致子类也没有。

#### 子类中删除的拷贝控制与基类的关系
- 如果基类的默认构造拷贝构造，拷贝赋值和析构是删除的或不可反问的，则子类的对应成员也是被删除的。因为编译器无法构造使用基类的函数构建子类的基类部分内容。
- 基类有不可访问或删除的析构，子类的合成的默认构造和拷贝构造是删除的。
- 编译器不会合成删除的移动操作，及时我们使用`default`显式要求合成。

如果基类的拷贝是被删除的，那么子类也必须自己定义拷贝才能被拷贝。同理移动操作也是如此。

因此一般基类没有默认构造，拷贝或移动构造时，子类一般也不会定义相应的操作。

### 派生类的拷贝控制

子类的构造函数调用父类构造函数是在初始化列表里构造的。

无论什么构造，子类构造默认调用的是父类的默认构造函数，如果想要调用其他的构造函数，需要显式的在初始化列表中编写。


在拷贝和移动构造中，不仅要拷贝和移动自己的资源，也要拷贝和移动基类的成员。

因此 拷贝和移动等操作中，需要显式的拷贝或移动父类资源。

### 派生类的析构函数
子类析构函数只负责释放自己的资源，父类资源会在执行完函数体后调用父类的析构函数。


### 继承中的构造函数
在`c++11`中，子类可以重用直接基类定义的构造函数。
一个类只继承直接父类的构造函数。
通过`using Base::Base;`来继承父类的全部构造函数，如果子类没有定义他们，则编译器会自动生成，生成构造函数就是把名字换成本类的名字，其他的参数列表之类的都和父类的对应构造一样，在初始化列表中调用父类的对应构造函数。
```cpp
struct Base{
  Base();
  Base(const Base &);
  Base(Base &&);
};
struct Derive:Base{
  using Base::Base;
  // 默认会生成拷贝和移动构造
  // 类似下面这样
  // Derive(const Derive &d):Base(d){}
}
```
`using`声明构造函数的时候不会改变构造函数的访问级别。`private`还是`private`，而且`explicit`和`constexpr`的属性也不会改变，如果基类构造是这样的属性，`using`过后还是这样的属性。

继承来的构造函数如果带默认实参会得到多个版本。
```cpp
struct Base{
  Base()=default;
  Base(int a,int b=1,int c= 1,int d=1){}
};
struct Derive:Base{
  using Base::Base;
  // 实际上内部有以下几个版本的构造函数
  // Derive(int a,int b = 1,int c = 1,int d = 1);
  // Derive(int a,int b,int c = 1,int d = 1);
  // Derive(int a,int b,int c ,int d = 1);
  // Derive(int a,int b,int c ,int d);
}

```
## 容器与继承
不允许容器内存放不同类型的元素，因此一般通过间接存储的方式存放对象到容器。

如果定义了一个类型的容器，那么容器内元素类型的子类如果被`push_back`进容器，其内容会被切掉，只有父类内容存入容器。


### 容器内应该放置指针
应该存放指针，因为子类指针会隐式转换为父类


但是指针操作很扰人，因此我们希望我们自定义的容器接收类对象，但是存放指针。

但是这又引出了一个问题，如果接收到的是对象的右值引用，需要移动，这时我们却没有办法新建对象，因为我们并不知道传入的对象的真正类型(可能是子类的对象)，因此我们没有办法分配内存(不知道分配多大).

因为运行时才分配内存，正好契合动态绑定，所以我们可以给基类加两个`virtual`的函数`clone`，一个是移动一个是拷贝，利用`new`分配对象后返回指针。

这时我们就可以通过对象来获取指针添加到自己写的容器类中了。
```cpp
Bulk_quote *Bulk_quote::clone() const & override { return new Bulk_quote(*this); }
Bulk_quote *Bulk_quote::clone() && override { return new Bulk_quote(std::move(*this)); }
void Basket::add_item(const Quote &sale) {
  item.insert(std::make_shared<Quote>(sale.clone()));
}
void Basket::add_item(Quote &&sale) {
  item.insert(std::make_shared<Quote>(std::move(sale).clone()));
}
```
注意，想要调用智能右值调用的重载函数需要使用`move`转换为右值。

**注意声明类似`set<T,decltype(compare)*> st`的时候一定不要忘记初始化该对象的比较器，否则插入元素的时候会发生段错误**

