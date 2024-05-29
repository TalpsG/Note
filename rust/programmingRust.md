# 类型
## 引用
引用可以视为`rust`的指针类型
`&mut `类似普通的指针，`&`类似常量指针，不可修改指向的值。
`rust`中也有裸指针，只存在`unsafe`代码块中

数组`[T;N]`,类型是`T`，长度是`N`
动态数组`Vec<T>`
数组切片引用`&[T]`

对数组和`vec`调用方法总是会隐式转换到切片然后调用。因为方法都是定义在切片上的。
切片总是以引用的方式来使用。
切片引用大小是双字，前半部分是指向第一个元素的指针，后半部分记录了切片中元素大小


## 字节串
`let method = b"get";`
上述代码就是一个字节串
# 所有权
## 移动数组内元素
1. `pop`弹出数组末尾的值，移动到新变量处
2. `swap_remove`先交换位置，将要移动的换到最后，再`remove`
3. `std::mem::replace`将要转移出来的值替换出来

## copy类型
一些不直接存储在栈上的数据都是不能`copy`的。

## rc和arc
`rc/arc`都是引用计数`arc`是线程安全的

```rust
use std::rc::Rc;
// Rust能推断出所有这些类型，这里写出它们只是为了讲解时清晰
let s: Rc<String> = Rc::new("shirataki".to_string());
let t: Rc<String> = s.clone();
let u: Rc<String> = s.clone();
```
类似`cpp`的智能指针。
但是不可以修改`rc`和`arc`引用的值

因为不可以`rc/arc`的不可修改性，实际上很少会出现共享指针的循环引用的现象，可以使用类似`cpp`的`std::rc::weak`来解决


## 引用
共享引用和可变引用

`rust`会对左操作数上的引用进行隐式解引用

`rust`中允许对引用重新赋值:
```rust
let x = 10;
let y = 20;
let mut r = &x;
if b { r = &y; }
```
### 允许引用的引用
```rust
struct Point { x: i32, y: i32 }
let point = Point { x: 1000, y: 729 };
let r: &Point = &point;
let rr: &&Point = &r;
let rrr: &&&Point = &rr;
```
在隐式解引用的时候会一直解引用直到追踪到最终的值。

当我们真的相比较引用的地址大小时，可以使用`std::ptr::eq(a,b)`来进行比较，但是引用类型必须相同(多层引用类型是不相同的)

### 共享和可变引用
可变引用是写引用，独占对象，共享引用是读引用，可以多个共享引用同时存在
当对对象进行读时，当作读引用处理，对对象写时会当作可变引用处理，因此写对象时不能存在其他引用。

## 表达式
### 块
块内是可以包含`fn struct use`的，封闭块与闭包不同，闭包可以看到所在作用域内的变量，而封闭块不行.

### match
`match`后面必须覆盖所有可能的表达式

### 循环

`loop for while`
`break`可以返回值，因此循环也可以用在赋值语句中
而且循环可以带有生命周期的标签，`break continue`也可以结合标签来进行自由的切换控制流


### return 
有永远不会返回的函数
```rust
fn never_return()->!{
  exit(0);
}
```
### 函数和方法调用
类型关联函数，就是结构体中实现的没有`self`参数的函数，需要通过类型来调用，比如`Vec::new()`，这类函数一般用来创建对象。


### 引用
在对引用使用运算符或者调用成员时会自动解引用，但当向使用对象的值的时候，需要手动`*`来解引用


### 类型转换
```rust
let x = 7;
let index = x as usize;
```
使用`as`来进行

- 数值类型可以相互转换，窄化转换会被截断部分数据.浮点到整形则会向零转换

### 闭包
类似`cpp`的`lambda`，也可以赋值给一个对象
`let is_even = |x| -> bool {x %2 ==0};`
而且可以像`cpp`中一样不写返回类型，自动推断类型


## 异常处理
### panic
`panic`发生在程序出现`bug`的时候比如
- 数组访问越界
- 除0
- 对返回值是`Err`的`Result`调用`expect`
等


### Result
`fn get()->Result<String,io::Error>;`
上述函数的返回值就是`Result`，他是一个枚举类型，实现类似
```rust
enum Result<T,E>{
  Ok:T,
  Err:E,
}
```
在调用返回`Result`的函数时，需要处理返回值后才能使用里面的值

最常见的方法是`match`进行模式匹配。
`unwrap_or(value)`解包数据，如果返回是`Ok`或者`Some`则返回内部的值，否则返回`value`
`unwrap_or_else(fn)`否则执行闭包

`is_ok() is_err()`返回`bool`值

`result.as_ref()`可以将`Result<T,E>`转换为`Result<&T,&E>`
同样的`as_mut`可以转换为可变引用
使用`as_ref`之类的函数可以在不消耗`result`的情况下访问`result`
`result.as_ref().ok()`

### result别名
在某些rust文档中可能会忽略`Result`的错误类型
`fn remove()->Result<()>`,这是因为在某些模块中定义了类型别名
`pub type Result<T>= Result<T,Error>`,此时我们如果`use std::module;`使用了这个模块，就可以直接使用该别名

### 打印错误
可以直接使用`println`.
对错误`err`可以调用`err.source`获取导致错误的`Option`，返回之后还可以继续调用`source`向上获取(如果存在的话)


### 传播错误
`?`可以传播`Result/Option`的出错的值。
当我们在函数中要传递多个不同类型的`err`时，有两种方法，一种是使用`thiserror`这个`crate`。
或者包装一个泛型类型
`type GenericError= Box<dyn std::error::Error+Send+Sync+'static>`
这样使用别名然后修改返回值类型即可，`?`会自动将错误转换成`GenericError`

## 结构体
关联常量
```rust
impl talps{
   const NAME:&'static str = "vector";
}
```



