# 动态内存
## 智能指针
`new`在堆区创建对象，返回对象的指针。`delete`接受一个堆对象的指针，销毁对象，释放内存。

由于使用裸指针容易出现内存泄漏等问题，标准库新提供了`shared_ptr`和`unique_ptr`以及`weak_ptr`用于指针的管理,智能指针负责自动释放所指对象。

### shared_ptr
`shared_ptr<string> p1;`
上面代码定义了一个空的智能指针`p1`，他与普通指针的使用方法一样。

`shared_ptr`和`unique_ptr`都支持的操作
1. 类似上述代码初始化空的智能指针
2. 如果指针没有指向一个对象则其`bool`值为0，反之为1 
3. `p.get()`获得指针的值，需要注意指针的释放
4. `swap(p1,p2)` `p1.swap(p2)`交换智能指针的值

`shared_ptr`独有的操作
1. `make_shared<T>(args)`，返回一个使用`args`初始化的`shared_ptr`
2. `shared_ptr<T> p(q)`，拷贝`q`的指针值，要求`q`必须能转换为`T*`，`q`的计数器加一
3. `p = q` ，两个指针都是`shared_ptr`可以相互赋值，会影响两个指针的计数值
4. `p.unique()`，判断指针计数值是否为1 
5. `p.use_count()`，返回与`p`共享对象的智能指针个数，很慢，仅用于`debug`

我们最通常生成`shared_ptr`的方法就是使用`make_shared`。
`auto p = make_shared<T>(args);`

当智能指针引用个数为1时被修改指向其他位置时，原对象将会被调用析构函数。

如果两个对象共享底层的数据，那么我们不能在销毁某个对象时(比如局部变量在函数执行完毕后的自动销毁)，简单的销毁其底层数据。(比如`Blob`类中存放`string`，`B1` `B2`都存放了同一个`string`,不能简单的释放`B1`或`B2`)
我们可以在类内实现的时候使用`shared_ptr`来管理数据，这样数据是在堆上创建的，不会受到作用域的影响，同时又有智能指针进行管理，在类之间赋值时，由于是指针赋值，所以不会产生拷贝的开销。

**`const`修饰的成员函数是构成重载的，因为我们需要常成员函数来供`const`对象来调用,因为`const`成员只能调用常成员函数**

**不用`explicit`修饰的构造函数可以在某些情况下让某个对象通过这个构造函数隐式转换成对应的类型**

### 直接管理内存
对于直接使用`new`和`delete`分配的对象，不能依赖类的拷贝，赋值和销毁的默认定义。
```cpp
int *p1 = new int; 
// 默认初始化，对于类来说，默认初始化调用默认的构造函数
// 对于内置类型来说，默认初始化意味着初始值未定义
int *p2 = new int();
// 值初始化，对于类来说，还是调用默认构造
// 对于内置类型来说，值初始化有着良好定义的值
auto p = new auto(obj);
// obj的类型决定了p的类型，利用obj初始化p指向的对象
```


类似的，也可以使用`new`创建`const`对象，但是`const`对象必须在创建时被初始化,默认构造也算初始化。同样的也可以销毁`const`对象。


**对于类来说，类内的内置类型如果没有被初始化，他们的值也是未定义的**

`new`在内存耗尽的时候会抛出异常`bad_alloc`,也可以通过给`new`传递参数`new (nothrow)int`来让`new`不抛异常，在分配失败的时候返回空指针


#### shared和new混用
`shared_ptr<double> p2(new double(22.33));`
我们可以使用`new`来初始化智能指针,**由于用指针初始化智能指针是通过构造函数实现的，而这个构造又是`explicit`的，所以不可以使用`p = new int`或其他类型转换的方式初始化智能指针**

```cpp
shared_ptr<int> p(q);
// 指针初始化
shared_ptr<int> p(u);
// p从unique指针u处接管对象的所有权
shared_ptr<int> p(q,d);
// q就是初始化用到的指针，d是释放对象所用的方法
```
与上述初始化的方法类似`p.reset`也有三种用法，分别是置空，利用其他指针改变指向，以及提供释放对象的方法。 `p.reset(q,d)`


**不要混用智能指针和普通指针,当使用智能指针绑定普通指针之后，就不要用普通指针来管理对象了**
反例如下
```cpp
void foo(shared_ptr<int> *p){}
int *a(new int(1024));
foo(shared_ptr<int>(a));
// 此处由于函数执行完毕 智能指针计数值为0,a就被释放了
int j = *a;
// 这行代码就是未定义的了，因为a指向的元素被释放了
```

#### 异常与智能指针
智能指针在发生异常的时候也会进行检查引用计数和释放内存。
而直接管理的内存不会自动释放，如果发生异常且没有捕获，则内存永远不会被释放了。


#### 智能指针tips
- 不使用相同的内置指针初始化多个智能指针
- 不`delete` `get()`返回的指针
- 不使用`get`初始化或`reset`智能指针
- 如果智能指针管理的不是动态内存，则需要传入一个删除器


### unique_ptr
与`shared_ptr`不同，`unique_ptr`在同一时刻智能有一个`unique_ptr`指向给定对象，该指针被销毁时对象也会被销毁。

而且没有`make_shared`之类的函数返回`unique_ptr`，我们需要将其绑定到`new`的对象上。

由于`unique_ptr`的特性，它不支持赋值以及拷贝。(`unique_ptr<int> p(up)`和`up1 = up2`是错误的)

```cpp
unique_ptr<int> u1;
// 空指针，如果非空，销毁时调用delete销毁对象
unique_ptr<int,D> u2;
// 空指针，非空销毁时会调用D类型的对象进行释放
unique_ptr<int,D> u3(d);
// 空指针，用d来代替delete,d是D类型的
u = nullptr; // 置空并释放原来的对象
u.release(); // 放弃对对象的控制权，返回指针，并置空u 
u.reset();   // 释放对象，如果提供了内置指针q参数，则指向q，否则置空
```

`release`返回的指针通常是用来初始化或赋值给其他智能指针的。

不能拷贝`unique_ptr`有一个例外，我们可以拷贝或赋值一个要被销毁的`unique_ptr`，例如下面的代码。
```cpp
unique_ptr<int> clone(int p){
  return unique_ptr<int> (new int(p));
}

unique_ptr<int> clone(int p){
  unique_ptr<int> ret(new int(p));
  return ret;
}
// 返回局部对象的拷贝
```
因为编译器知道返回的对象要被销毁，所以可以执行特殊的拷贝，后面会讲到。

在传递`unique_ptr`的删除器时，我们要注意在创建`unique_ptr`时就要确定删除器的类型，这关系着`unique_ptr`的类型。 



### weak_ptr
简单的来讲`weak_ptr`就是绑定在`shared_ptr`上的智能指针，但是`weak_ptr`不增加`shared_ptr`的计数值。

`weak_ptr`的操作如下
```cpp
weak_ptr<int> w;
weak_ptr<int> w1(sp); // 使用shared_ptr 初始化
w = p; // p可以是shared或weak 
w.reset(); // 置空
w.use_count(); // 返回共享对象的shared个数
w.expired(); // 如果use_count== 0 返回true,否则false
w.lock(); // 如果共享对象没有shared_ptr则返回空指针，否则返回一个指向该对象的shared_ptr
```

由于`weak_ptr`不影响对象的生命周期，我们可以利用他来处理对资源的访问,通过`lock`返回的指针可以判断对象是否已经被释放，没有被释放也会被智能指针所管理。


## 动态数组
可以通过`new`来新建数组。
`int *p = new int[get_size()];`
虽然新建了数组，但是我们得到的仅仅是一个指针，无法对他使用`begin` `end`等函数。


动态分配的数组的初始化：
- `int *p = new int[10];`代表了10个没有初始化的`int`
- `int *p = new int[10]();`代表了10个值初始化的`int`
- `int *p = new int[10]{1,2,3,4};`前四个使用给定的初始化器，后面的进行值初始化

释放动态数组：
`delete [] p;`逆序销毁`p`中的元素，从后向前。

#### 智能指针和动态数组
标准库提供了可以管理`new`分配的数组的`unique_ptr`
`unique_ptr<int[]> up(new int[10]);`

而`shared_ptr`不支持直接管理动态数组，如果需要，必须自己实现一个删除器
`shared_ptr<int> sp(new int[10],[](int *p){delete[] p;})`

### allocator 分配器
`allocator`简单来讲就是纯纯用作分配内存的一个类，他是一个模板类,大概是这样用的
```cpp
allocator<string> alloc;
auto const p = alloc.allocate(n);
```

上述代码分配了`n`个`string`对象，并返回了一个指针.

下面是`allocator`的其他操作
- `a.deallocate(p,n)` 释放对应的内存，`p`指向的内存必须是`allocator`分配的且大小必须等于`n`，在调用`reallocate`之前必须该块内存中的每个对象调用`destroy`
- `a.construct(p,args)`,在`p`处使用传入的`args`来调用构造函数创建一个对象
- `a.destroy()`, 对`p`处的对象执行析构函数


**使用未构造的内存是未定义行为**

#### 拷贝和填充函数
```cpp
uninitialized_copy(b,e,b2); // 从迭代器b，e之间拷贝到b2
uninitialized_copy_n(b,n,b2); // 从迭代器b开始，拷贝n个到b2
uninitialized_fill(b,e,t); // 迭代器b，e之间,填充为t
uninitialized_fill_n(b,n,t); // 从迭代器b开始，创建n对象，值为t
```
这里的`uninitialized_copy`类似`copy`返回拷贝后的新位置的迭代器。


### 文本查询程序
- `map<string,set<int>>`用来映射单词和出现行号
- `istringstream`用来分解单词
- `vector<string>` 用来存放文件，下标作为行号
- `TextQuery`类包含`vector`和`map`，用来保存文本以及单词出现次数等信息。
- `QueryResult`保存查询结果

