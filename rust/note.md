# 基础入门
## 写代码之前
`rust`项目分为两个类型，`lib`和`bin`

创建新项目`cargo new project-name`

构建项目并运行
1. `cargo run`
2. `cargo build`仅构建,之后手动执行`./target/debug/project-name`

在构建项目时，默认是在`debug`模式下，编译速度很快，但是运行较慢，并未做过多优化。当我们的项目已经完成，需要发布时，可以加上`--release`选项。

`cargo check`可以快速的检查代码是否能够编译，若工程过大编译速度较慢可以使用`check`来快速检查。

## cargo.toml 和cargo.lock
前者是`cargo`的项目描述文件，存储了项目的元数据。
后者一般不用修改.

### package
该段落记录了项目的描述信息，包括版本号，名称等。
### dependency
后面再说
## 变量
`let`的变量都是不可变的，可变的要特殊声明`mut`

变量都是绑定的值，可以理解成变量名是一张贴纸，可以贴到内存上。

编译器会忽略未使用的`_`开头的变量，因为未使用的变量正常情况下会报错.

### 解构：
`let (a,mut b):(bool , bool ) = (true,true);`
类似`python`和`c++ tuple`的解包

### 常量：
不允许`mut`，因为编译后就确定值了，不会再改变了。
且常量不能用`let`，要用`const`声明，必须标注类型
`const a:u32 = 23;`

### 变量遮蔽
在内部作用域声明的变量与外部的同名会遮蔽住外部的变量

### 返回值
不用显式的写`return`，只需要将返回的值单独写一行，不写分号即可。
返回值类型类似`c++`后置的方式。


## 基本类型
算术类型:`i8-i64,isize,u8-u64,usize,f32,f64`
字符串:字面值和切片`&str`
布尔类型
字符类型:`unicode` 4字节
单元类型:`()` ~~很奇怪~~


`rust`是静态类型语言，虽然很多时候可以推导出类型，但是某些情况下必须由程序员来标注变量的类型。

```rust
let c = "42".parse::<i32>().expect("Not a Number!");
println!("{}", c);
```

算术类型类似`c`语言，`isize,usize`是架构相关的类型。
默认使用`i32`

算术类型在`debug`模式下构建会检查溢出，而`release`下不会

可以显式的处理溢出
- `wrapping_add`等按照循环处理溢出
- `checked_add`等溢出时返回`none`
- `overflowing_add`等返回结果和一个指示是否溢出的值
- `saturating_add`等返回计算结果，计算结果在溢出时返回边界值

浮点数默认使用`f64`，在现代计算机中`f64`的运算开销和`f32`几乎相同。

`NaN`，当我们对负数取平方根就会得到该值，所有与`NaN`计算的结果都是`NaN`
，可以使用`x.is_nan()`来显式的判断是否是`NaN`




### 运算符
和`c`几乎一样

### range
`1..=5`是1-5.
`1..5`是1-4
序列只允许用于数字和字符类型


### As
使用`As`可以实现类型转换，一般用于原始类型之间，也可以对指针使用。


### tips
1. 数据类型标注可以后置`let x = 32i32;`
2. 长的数字可以分割易于观察`let x = 1_0000_0000;`
3. 取绝对值`x.abs()`

## 字符布尔和单元类型

字符都是`unicode`，四字节
布尔类型一个字节

单元类型: 这个类型只有一个`()`值,单元类型并不占用内存，`main`函数返回的就是单元类型。
这类函数有个特殊的名字`diverge function`发散函数。


## 表达式
语句不会返回值，表达式可以代替返回值
可以用块语句来给变量赋值
```rust
let a = {
  let b = 5;
  b + 1
};
```
上述例子中`a`为6

表达式加分号则会返回单元类型

## 函数

1. 函数定义在哪里都行
2. 函数参数类型要显式标注
3. 表达式可以在函数的最后一行充当返回值
4. 使用`return`可以提前返回
5. 返回值是`->!`的函数被称为发散函数，永远不会正常返回,通常用于无限循环或者中之程序运行的函数中

## 所有权和借用
`rust`无`gc`，靠编译器在编译时进行内存管理

所有权原则
1. 每个值都被变量拥有，变量被成为值的所有者
2. 一个值只能有一个所有者
3. 所有所有者都离开作用域时，值将被丢弃

### 简单的string  
`let a = "123"`a的值是硬编码进程序的，字符串值运行时无法改变，a的类型为`&str`

我们可以使用字面值来创建`string`类型
`let s = String::from("abc")`

这个字符串存储在堆上

## 继续所有权
```rust
let a = String:from("abc");
let b = a;
```
此处的赋值类似`c++`的移动语义，堆上`string`的所有权被转移到了`b`上，`
a`不能在使用堆上的对象.

当我们需要进行深拷贝的时候可以调用`clone`方法

类似`i32`这种类型的变量默认做的就是深拷贝，因为深浅拷贝开销并没有过多差距.
`rust`中有个`copy`的特征，当对象有这样的特征时，赋值就会是深拷贝
所有算术类型还有`char`都可以`copy`，`tuple`的成员都是`copy`的话也是如此。
**不可变引用`&T`也可以`copy`比如字符串常量值初始化的变量就是`&str`，但是可变引用`&mut T`不可以**


### 函数调用的所有权
```rust
fn takes_ownership(s: String) {
    println!("{}", s);
}
fn return_ownership(s: String) -> String {
    println!("{}", s);
    s
}
```
`takes_ownership`被调用时，传入的实参所有权转移到函数内部。函数完毕后并未返回所有权，因此函数调用结束后调用方无法再使用实参。
`return_ownership`相反，函数返回了所有权，调用方可以接收所用权。

## 引用与借用
`let r1 = &var1;`引用不改变所有权,使用时需要解引用`*r1`

`let (a,ref b) = (x,y);` 
在解包的时候会使用`ref`来声明引用
```rust
fn main() {
    let s1 = String::from("hello");
    let _len = cal(&s1);
}
fn cal(s: &String) -> usize {
    s.len()
}
```
我们可以定义函数参数为引用，这样调用函数就不需要来回转移所有权了。

引用分为可变引用`&mut T`可不可变引用`&T`

可变引用必须出现在可变变量上，且同一时刻只能有一个.

不可变引用可以有多个，可变引用和不可变引用不能同时存在

### 引用作用域
引用作用域和变量不同，引用的作用域一直持续到最后一次使用引用的地方。
### 借用
借用是一个概念，通过引用来使用变量的过程被称为借用

## 复合类型
```rust
#![allow(unused_variables)] 
// 表示编译器忽略未使用变量的警告
#[allow(dead_code)]
// 表示编译器忽略未使用的变量函数等
fn read(f: &mut Fi
    unimplemented!()
    // 表示函数未实现
}
```
## 字符串

### 切片
```rust
let a = String::from("hello");
let h = &s[0..2];
```
上述代码中`h`是字符串`a`中一部分的引用,类型是`&str`类型的，和字符串字面值的一样,不可变引用

做切片的时候要注意切片索引必须落在字符之间，不能落在字符内部。

```rust
fn main() {
    let mut s = String::from("hello world");
    let word = &s[0..5];
    s.clear(); // error!
    println!("the first word is: {}", word);
}
```
上述代码中`s.clear()`通过自身的可变引用来修改内容，而下方使用了不可变引用，可变可不可变同时出现了，因此编译器会报错

### 字符串
`rust`的字符串使用`unicode`编码，因此字符的长度不是固定的

`rust`语言级别只有一种字符串类型`str`,常常以引用形式出现`&str`.
标准库实现了`String`类型


### String -> &str 
- `String::from("hello,world");`
- `"hello world".to_string()`

在需要`&str`的函数中传入`String`也是可行的，这里会发生隐式转换后面再讲

### 字符串操作
`string`类型是可变字符串，我们可以进行以下操作
- `push`在原来的字符串追加字符，`push_str`追加字符串字面值
-  类似的`insert`和`insert_str`分别是插入字符和字符串字面值
-  `replace(old,new)`将会将字符串内的`old`改为`new`,且不会修改原字符串，会返回新的字符串,`relpacen`则多一个数字参数，代表要替换前几个子串
- `replace_range(1...5,str)`替换范围内的字符串为新的字符串，会在原字符串上操作
- 删除操作
  - `pop`删除最后一个字符，原字符串操作
  - `remove(index)`删除索引处的那个字符，注意如果索引处不在字符边界则会报错，原字符串操作
  - `truncate`，删除某个位置开始到最后的全部字符，原字符串操作
  - `clear`清空字符串，原字符串操作
- 连接操作
  - `+/+=`，右侧操作数**必须是字符串的切片类型**，返回新的字符串,这里是自动调用了标准库里`add`方法,此处第一个操作数的所有权会被转移，传递给返回值。
    - 支持链式的加法，加法的结果返回`string`
  - `format!("{} {} !",s1,s2)`,适用于`string`和`&str`


可以使用`\x`来转译字符


### 操作`utf8`字符串
```rust
for c in "中国人".chars(){
  println!("{}",c);
  // 按字符输出 中 国 人
}
for c in "中国人".bytes(){
  println!("{}",c);
  // 按字节输出
}
```

想按照字符获取字串是比较复杂的事情，标准库并没有实现

## 元组
`let tup:(f32,f64,i32) = (500,6.2,1);` 

可以用`tup.0`来访问第几个元素

模式匹配：
即解包`let (x,y,z) = tup`

## 结构体
可以给内部的成员起名字.
```rust
struct User {
    active: bool,
    username: String,
    email: String,
    sign_in_count: u64,
}
let user1 = User {
    email: String::from("someone@example.com"),
    username: String::from("someusername123"),
    active: true,
    sign_in_count: 1,
};
```
结构体初始化时每一个字段都要初始化，顺序可以不同。

结构必须被定义为`mut`，其成员才可以被修改。
`rust`不支持将结构体成员定义为`mut`的操作
### 结构体赋值
```rust
struct person{
  name:String,
  id : i32,
}
let p1 = {
  name:"wwt".to_string(),
  id :32
};
let p2 = {
  "Wwt".to_string(),
  32,
};
let p3 = {
  name:"talps".to_string(),
  ..p2
}
```
我们可以使用已经存在的结构体为新的结构体变量赋值 `..p2`,此操作会转移`p2`的部分所有权，因此`p2`整体不再能够使用，但是部分没有被转移所有权的成员还可以使用,实现了`copy trait`的也可以继续使用，因为赋值时没有转移所有权而是复制了一份

使用与结构体成员同名的变量初始化对应成员时，不用显式标注成员名

### 元组结构体
`struct Color(i32,i32,i32);`

### 单元结构体
`struct unitstruct;`
没有成员，当我们不关心类型内容只关心行为时可以使用单元结构体

### 结构体的生命周期
当结构体内成员有引用时，结构体变量的生命周期要小于引用的变量的生命周期

### 打印结构体
`dbg!(&)`
`#[derive(Debug)] + "{:?}"`

## 枚举类型
```rust
enum PokerSuit{
  Clubs,
  Spades,
  Diamonds,
  Hearts
}
let heart = PokerSuit::Hearts;
```
上述的枚举类型定义和使用很像`cpp`的枚举类,我们也可以给第一个枚举成员一个初值，后面的枚举成员就会递增，类似`c`语言的枚举。因此我们也可以对枚举类型使用`as`转换成整数类型
下面就不像了，因为我们可以给枚举类型添加上其他的数据类型
```rust
enum PokerCard{
  Clubs(u8),
  Spades(u8),
  Diamonds(u8),
  Hearts(u8),
}
let c = PokerSuit::Clubs(15);
```
我们可以给枚举类型成员添加任何的数据类型


我们使用枚举类型搭配成员上的数据类型，可以提高程序的内聚性，很多时候可以传入枚举类型的参数，在函数内部进行分发.


### option
用来代替`null`的
```rust
enum Option<T>{
  Some(T),
  None,
}
```
`option`包含两个成员

我们可以直接使用`Some`或`None`来创建`Option<T>`
当使用`None`时，变量必须标注出`Option`的类型
```rust
let a = Some(5);
let b :Option<String> = None;
```

当函数返回`Option`时，我们可以使用`match`来分派操作
```rust
fn plus_one(x:Option<i32>)->Option<i32>{
  match x{
    None => None,
    Some(i)=> Some(i+1),
  }
}
```
上述代码将传入的`option`加一返回了

## 数组
数组是`rust`的基本类型,固定长度.
`let a = [1,2,3,4,5]`
`let b:[i32;5] = [1,2,3,4,5]`
`let c = [5;5]`
`c`的内容就是5个5

可以通过索引来访问数组元素`a[5]`

`rust`在运行时会检查数组访问是否越界

当需要声明重复元素的非基本类型的数组时，需要使用`from_fn`
```rust
fn main() {
    let str_arr: [String; 5] = std::array::from_fn(|_i| String::from("wwt"));
    println!("{:#?}", str_arr);
}
```
### 数组切片
`let slice:&[i32] = &a[1..5];`


## 流程控制
### if
### 循环
`for i in 1..=5`
`for item in &collection`  
`for item in &mut collection`
在对集合使用`for in`时常常使用引用，否则所有权后被转移到`for`内，后面无法使用集合了。

`while condition`

`loop`
当使用`loop`时我们可以在`break`后面加一个值当作循环的返回值
因此会有这样的语句
```rust
let a = loop{
  if b == 0 {
    break 100;
  }
  b = b-1;
}
```
利用下标访问数组元素会增加运行时检查边界的开销

### 循环标签
`break continue`可以配合循环前的标签使用
```language
'outer' : loop{
  'outer2' : loop{
    if condition {
      break 'outer'
    }
  }
}
```
上述代码可以直接跳出`outer`也就是最外层的`loop`
## 模式匹配
`match`类似`switch`
```rust
match target{
  case1 => exp1,
  case2 =>{
    statements;
    exp2
  },
  - => exp3,
}
```
与`switch`不同的是`match`要求每个分支都要有返回值

使用模式可以对`option` 以及枚举类型的不同成员进行处理

`match`需要对每一个分支进行处理，可以用一个变量放在最后来处理全部剩下的分支，也可以使用`_`
`_ => exp`或`other => exp`

如果分支没有要返回的值，就返回单元类型`()`即可。

### if let
对于只需要匹配一个值的情况`match`有点啰嗦了，可以使用`if let`
```rust
let v = Some(3u8);
if let Some(3) = v{
  println!("three");
}

```

### matches! 宏
`matches!(x,MyEnum::Foo)`会进行模式匹配，返回匹配的结果`true`或者`false`

### option 
```rust
fn main() {
    let five = Some(5);
    let none: Option<i32> = None;
    let six = increment(five);
    let null = increment(none);
    println!("{:?} {:?}", six, null);
}
fn increment(op: Option<i32>) -> Option<i32> {
    match op {
        None => None,
        Some(i) => Some(i + 1),
    }
}
```

### 模式
```rust
let mut stack = Vec::new();
stack.push(1);
stack.push(2);
stack.push(3);

while let Some(i) = stack.pop() {
    println!("{}", i);
}
```
`while let`类似`if let`当条件满足时一直就继续循环

其实对于`let if match`还有函数参数匹配和解包都是模式匹配的过程，必须全部覆盖才能通过编译，而`if let`和`while let`只需要匹配到一个模式即可

### 模式匹配例子
```rust
let x = 5;
match x {
    1..=5 => println!("one through five"),
    _ => println!("something else"),
}
```
模式匹配可以让单个的值匹配序列内的值


使用`_`来忽略部分值时，`_`不会绑定到对应的值上
类似的也可以使用`..`来忽略剩余值或者开头的部分值
`let (x,..) = (1,2,3,4);`
`let (..,x) = (1,2,3,4);`
`let (x,..,y) = (1,2,3,4);`

匹配守卫
```rust
let num = Some(4);
match num{
  Some(x) if x<5 => println!("less than 5 :{}",x),
  Some(x) => println!("{}",x),
  None => (),
}
```

### @绑定

```rust
enum Message {
    Hello { id: i32 },
}

let msg = Message::Hello { id: 5 };

match msg {
    Message::Hello { id: id_variable @ 3..=7 } => {
        println!("Found an id in range: {}", id_variable)
    },
    Message::Hello { id: 10..=12 } => {
        println!("Found an id in another range")
    },
    Message::Hello { id } => {
        println!("Found some other id: {}", id)
    },
}
```
当`@`后面的模式符合时，将前面的名字绑定到变量上

需要注意的是在`@`后面的 模式如果有`| &`需要括号阔起来，否则会认为是多个模式，编译器报错没有匹配全部的模式

对引用进行模式匹配得到的是值而非引用,想要匹配引用需要如下的代码
```rust
let ref_to_num = &5; // 我们有一个指向数值的引用
match ref_to_num {
    &val => println!("We got the value {}", val), // 解构引用，获取原始值
    _ => println!("This is not a reference!"),
}
```
上述代码中由于`ref_to_num`是个引用，所以下方匹配引用使用的是`&val`

在对变量值进行模式匹配时想要匹配引用就需要用`ref`
```rust
let number = 5; // 我们有一个普通的数值

match number {
    ref r => println!("Got a reference to {}", r), // 创建一个引用到匹配到的值，不夺取它的所有权
    _ => println!("Mismatch"),
}
```

## method

方法往往是跟着结构体，枚举类，`trait`一起使用

`rust`的方法定义和`cpp`不同，方法实现单独写在`impl`代码块内

```rust
struct Circle {
    x: f64,
    y: f64,
    radius: f64,
}
impl Circle {
    fn new(x: f64, y: f64, radius: f64) -> Circle {
        Circle { x, y, radius }
    }
    fn area(&self) -> f64 {
        std::f64::consts::PI * (self.radius * self.radius)
    }
}
```
注意参数`self`的使用带来的所有权概念
- `self`:会将所有权带到方法内
- `&self`: 借用不可变引用
- `&mut self`:可变引用



结构体内的方法可以和字段名重名

我们也可以为枚举类型实现方法

### 关联函数
在`impl`中但是没有`self`的函数被称为关联函数，一般用于构造新对象



## 泛型和特征
结构体中的泛型
```rust
struct Point<T>{
  x:T,
  y:T,
}
```

枚举泛型中的卧龙凤雏`Option`和`Result`
`Option`关注的是有没有值，而`Result`则关注的是值的正确性
```rust
enum Result<T,E>{
  Ok(T),
  Err(E),
}
```
方法中使用泛型：
```rust
impl<T> Point<T>{
  fn x(){

  }
}
```
我们也可以为具体的泛型类型实现方法
```rust
impl Point<i32>{
  fn foo()->i32{
    5
  }
}
```

### const泛型
`cpp`也有类似的泛型，在`cpp`中的模板函数接受数组引用时，不同长度的数组，类型不同，其引用类型也不同。因此有了如下的实现
```cpp
template <typename T, size_t N>
void myFunction(const T (&array)[N]) {
    // 在这里，array 是对一个大小为 N 的数组的引用
    for (size_t i = 0; i < N; ++i) {
        // Do something with array[i]
    }
}
```
类似的`rust`的`const`泛型类似
```rust
fn display_array<T: std::fmt::Debug, const N: usize>(arr: [T; N]) {
    println!("{:?}", arr);
}
```

`rust`的泛型是零成本的抽象，不过一切皆是`trade-off`，为了运行时的零成本，在编译时会生成多份类似的代码(其实就是`cpp`的模板元).

`fn add<T:std::ops::Add<Output = T>>(a:T,b:T)->T{}`

## trait
类似接口的东西

定义与实现`traint`
```rust
pub trait Summary{
  fn summarize(&self)->String;
}
pub struct Post{
  pub title:String,
  pub author:String,
}
impl Summary for Post{
  fn summarize(&self)->String {
    
  }
}
```
我们可以在定义时实现`trait`，这样在其他类型中可以使用默认实现。
`impl Summary for Post{}`


### 孤儿规则
当我们想为某个类型实现某个`trait`时，必须保证类型或`trait`至少有一个在当前的作用域内。

### 特征约束
我们可以用特征作为函数的参数来约束函数实参
`fn notify(item:&impl Summary){}`
上述的代码是一个语法糖，正常的写法是
`fn notify<T:Summary> (item:&T){}`
当函数参数变多且类型都不同时，使用语法糖很方便，但是当参数有多个但是类型相同时，语法糖无法满足需求
`fn notify<T:Summary> (i1:&T,i2:&T){}`

### 多重约束
`fn notify(item:&(impl trait1+trait2)){}`
`fn notify<T:trait1+trait2>(item:&T){}`

我们也可以将约束后置到返回值之后处
```rust
fn some<T,U>(t:&T,u:&U)->i32
  where T:Display + Clone,
        U:Clone + Debug{

}
```
通过泛型和`trait`约束，我们可以为特定类型的结构体对象生成特定的方法
```rust

struct Pair<T> {
    x: T,
    y: T,
}
impl<T: Display + PartialOrd> Pair<T> {
    fn cmp_display(&self) {
        if self.x >= self.y {
            println!("The largest member is x = {}", self.x);
        } else {
            println!("The largest member is y = {}", self.y);
        }
    }
}
```

### 返回trait 
函数可以返回`impl trait`来表示函数返回一个实现了某个特征的类型
```rust
fn foo(i: i32) -> impl Jia {
    if i > 5 {
        Integer { x: 10 }
    } else {
        Integer { x: 0 }
    }
}
```
返回值写`impl trait`的方式只能返回一个类型，即返回值只能是一个类型。如果需要实现不同的类型，则需要特征对象。
### derive 派生特征
`#[derive(Debug)]`会自动实现默认的`Debug`特征代码。

## 特征对象
特征对象的目的是解决`fn foo()->impl trait;`函数只能返回一个类型的问题。
动态类型`dyn trait`实现了某个特征的类型

动态类型类似运行时多态，有运行时开销。
`fn foo(x:&dyn trait1){}`
`fn foo(x:Box<dyn trait2>){}`

特征对象的大小运行时确定。

特征对象调用特征相关信息时采用虚表的方式进行，对特征对象的引用有两个指针，一个指向特征对象的实例，另外一个指向虚表，虚表上有特征的方法等


**利用`Box<dyn trait>`我们可以返回一个特征对象，因为特征对象的大小是固定的**

```rust
trait Bird {
    fn quack(&self) -> String;
}
struct Duck;
impl Bird for Duck {
    fn quack(&self) -> String {
        "duck duck".to_string()
    }
}
struct Cat;
impl Bird for Cat {
    fn quack(&self) -> String {
        "cat cat".to_string()
    }
}
fn foo(i: i32) -> Box<dyn Bird> {
    if let 1 = i {
        Box::new(Cat)
    } else {
        Box::new(Duck)
    }
}
fn bar<T>(a: T) -> String
where
    T: Bird,
{
    a.quack()
}
```
### 特征对象的限制
只有对象安全的特征才能有特征对象
- 方法返回类型不能是`Self`
- 方法没有泛型参数

当方法有泛型参数时特征对象会抹除泛型参数的信息。

### 一些特征对象的使用方法
`let arr:[Box<dyn trait1>;2] = [Box::new(Duck),Box::new(Swan)]`


特征对象通过对`impl trait`的对象进行包装实现了返回不同类型的需求，但是也增加了运行时开销。

## 深入理解特征
关联类型
就是在特征定义的语句块内 自定义一个类型，这样就可以在特征的方法签名中使用该类型

```rust
pub trait Iterator{
  type Item;
  fn next(&mut self) -> Option<Self::Item>;
}
```
在实现`trait`的时候指定对应的关联类型


### 默认泛型参数
```rust
trait Add<RHS=Self>{
  type Output;
  fn add(self,rhs:RHS)->Self::Output;
}
```
上面代码是`Add`的实现，给`rhs`默认的参数，并且有一个关联类型


### trait 和泛型
`trait`中也可以有泛型，比如
```rust
trait Add<RHS=Self>{
  type Output;
  fn add(self,rhs:RHS)->Self::Output;
}
```
上述代码是标准库的加法`trait`，方法参数有多个且类型不定时，就可以使用`trait`加泛型的方式实现

### trait的同名方法
`trait`有同名的方法是很正常的，但是如果和类型本身的方法也同名了。则调用时优先调用类型本身的同名方法

对于实现了多个带有同名方法的`trait`的类型，想要调用某一个`trait`的方法，需要显式的调用
```rust
trait Pilot {
    fn fly(&self);
}

trait Wizard {
    fn fly(&self);
}

struct Human;

impl Pilot for Human {
    fn fly(&self) {
        println!("This is your captain speaking.");
    }
}

impl Wizard for Human {
    fn fly(&self) {
        println!("Up!");
    }
}

impl Human {
    fn fly(&self) {
        println!("*waving arms furiously*");
    }
}
fn main(){
  let a = Human;
  Wizard::fly(&a);
  Polit::fly(&a);
}
```

但是`impl`中也有没有`self`参数的关联函数，此时如何调用呢？
**此时需要使用完全限定语法**
```rust
trait Animal {
    fn baby_name() -> String;
}

struct Dog;

impl Dog {
    fn baby_name() -> String {
        String::from("Spot")
    }
}

impl Animal for Dog {
    fn baby_name() -> String {
        String::from("puppy")
    }
}

fn main() {
    println!("A baby dog is called a {}", Dog::baby_name());
}
```
上述代码中`Dog::baby_name()`可以调用`dog`自己实现的方法，但是如果我们不知道`impl Animal`的类型是`dog`呢？
直接通过`Animal::baby_name()`调用会报错，因为无法从`Animal`判断出对象真正的类型

`<Dog as Animal>::baby_name()`,这样调用就会调用`dog`实现的`trait`里面的`baby_name`方法


### 特征的特征约束
与泛型中的特征约束类似，此处的特征约束说明了本特征的实现需要以其他特征为前提
```rust
trait OutlinePrint:Display{
  fn outline_print(&self);
}
```
因此实现`trait`时需要一个一个实现，如果`trait`的约束没有实现就要先实现约束`trait`
### 外部类型实现外部特征
有一个方法可以绕过孤儿规则,`newtype`
实际上就是写一个包装类型，然后实现包装类型的特征。
```rust
use std::fmt;
struct Wrapper(Vec<String>);
impl fmt::Display for Wrapper {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "[{}]", self.0.join(","))
    }
}
fn main() {
    let w = Wrapper(vec![String::from("hello"), String::from("world")]);
    println!("{}", w);
}
```
这样做虽然实现了`display`但是使用起来还是有点麻烦，毕竟需要`self.0`来访问数组。
之后我们可以实现一个`Deref`的`trait`，类似`cpp`中的重载解引用



## vector
```rust
let v:Vec<i32> = Vec::new(); 
let v2 = Vec::new(); 
// 单独使用编译器会报错
v2.push(1); 
// 结合上面的语句使用可以让编译器推导出v2的类型
let v3 = vec![1,2,3];
// 可以赋初值
```

###  操作
`get`:返回一个`Option`因此需要`match`进行处理
`push`:
`&a[2]`:获取第三个元素的不可变引用
`extend`:给结尾添加数组元素
`insert(index,item)`
`remove(index)`
`retain(|x| *x > 10)`:保留符合条件的元素
`append(&v)`:在数组后更新清空参数的数组
`truncate(num)`:截断数组，只保留部分
`drain(1..n)`:删除范围内的元素，返回被删除数组的迭代器
`collect`将迭代器转换为集合
`split_off(n)`:在`n`处分裂数组，返回后半部分

### 存储不同的元素
可以借助枚举类型或特征对象来存储不同类型的元素

在实际场景中，使用特征对象要比枚举更常见，因为枚举类型不能动态的增加类型
### vector的排序
`vec.sort_unstable()`
`vec.sort()`

```rust
let mut people = vec![
    Person::new("Zoe".to_string(), 25),
    Person::new("Al".to_string(), 60),
    Person::new("John".to_string(), 1),
];
people.sort_unstable_by(|a, b| b.age.cmp(&a.age));
```

## hashmap
```rust
let mp = HashMap::new();
let mp2 = HashMap::new();
let arr = vec![
  ("1".to_string(),1),
  ("2".to_string(),2),
  ("3".to_string(),3),
];
let mp3:HashMap<_,_> = arr.into_iter().collect()
```

`vector(key,value)`转`hashmap`时可以先将其转换成迭代器，然后转成集合
` let mp3:HashMap<_,_> = arr.into_iter().collect() `:

 
### 所有权
`map`插入时如果传递值且类型实现了`copy`则没有所有权问题，否则所有权被移动
传递引用则要确保引用的对象的生命周期要比`hashmap`要长

### 查询 get
`get`方法用于查询，传入`key`类型的引用，返回一个`Option`存放值的引用。
如果我们需要值类型，则可以使用
`get(&key).copied().unwrap_or(default_value)`.
`copied`会复制值出来，返回一个存放拷贝了值的`Option`，`unwrap`将其拨开，如果`None`就返回默认值。


### 更新
1. `insert`传入参数类型是引用类型,或是值类型，值类型涉及所有权的转移。不存在会插入,存在则会更新`value`
2. `entry(&key)`返回一个`kv`对,~~好像是个`entry`类型~~
3. `or_insert`，一般接着上面的`entry`使用，在没有查找到是插入，返回`value`的可变引用。


## 生命周期和dangling指针
```rust
let r;
{
  let x = 10;
  r = &x;
}
println!("{r}");
```
引用了已经被释放的变量被称作悬垂指针


### 函数中的生命周期
```rust
fn longest(x: &str, y: &str) -> &str {
    if x.len() > y.len() {
        x
    } else {
        y
    }
}
```
上面的代码会报错，因为编译器无法在编译时得到返回值的生命周期信息，因此需要程序员显式的进行标注

`fn useless<'a>(first: &'a i32, second: &'a i32) {} `
上面的代码中表示`first`和`second`至少活的和`a`一样久

这样的标注过后，并没有改变参数的生命周期。而是告诉编译器，在不符合该条件时拒绝编译通过

### 结构体中的生命周期
只要有引用的地方都有生命周期
```rust
struct ImportantExcerpt<'a> {
    part: &'a str,
}
```
上述代码的意思是，结构体内`part`的生命周期至少要和结构体本身一样

### 消除规则
大部分情况下不需要程序员手动标注生命周期，因为编译器很聪明，可以替我们做了。
编译器按照三条规则进行推断
1. 每个引用自己有一个生命周期`'a`
2. 若只有一个输入生命周期，则输入生命周期会赋给所有输出生命周期
3. 如果有多个输入生命周期，有一个是`&self/&mut self`则`self`生命周期被赋给所有输出生命周期

当编译器无法推断时，就需要我们手动进行标注了.

### 方法中的生命周期
```rust
struct ImportantExcerpt<'a> {
    part: &'a str,
}
impl<'a> ImportantExcerpt<'a> {
    fn level(&self) -> i32 {
        3
    }
}
```
生命周期的标注也属于结构体类型的一部分，因此`impl`也要写生命周期标注
```rust
impl<'a: 'b, 'b> ImportantExcerpt<'a> {
    fn announce_and_return_part(&'a self, announcement: &'b str) -> &'b str {
        println!("Attention please: {}", announcement);
        self.part
    }
}
```
生命周期约束`'a:'b`代表着`a`比`b`活得久

### 静态生命周期
`'static`活得和程序一样久，分为`T:'static` 和`&'static`.

## 错误处理
`Result`用于可恢复错误
`panic`用于不可恢复的错误

### panic
可以主动调用`panic!("")`
`panic`时有两种终止方式
1. 栈展开:使用`RUST_BACKTRACE=1 cargo run`来执行程序，此时终止时会展开发生`panic`的函数调用栈
2. 直接终止:直接终止，不清理数据。

可以在`Cargo.toml`中添加以下内容实现`release`模式下直接终止
```toml
[profile.release]
panic = 'abort'
```

子线程`panic`不影响其他线程，主线程`panic`会直接终止程序

### Result
有些函数比如`File::open(filename)`就会返回一个`Result`我们可以用返回值去做`match`来执行不同的行为。也可以直接`unwrap`获取其中的值，如果`open`失败，`unwrap`会触发`panic`。
`expect`类似`unwrap`，只是`expect`接受一个字符串，传递给了`panic`

如果的`Result`得到的而是`Err(e)`我们可以向上层调用去抛出`err`，让上层进行处理


### 自动传播符号
在返回`result`的函数后面加一个`?`此时函数如果返回`Err`则会自动向上抛`Err`.否则返回`OK`里面的值
而且`?`是可以链式调用的
```rust
use std::fs::File;
use std::io;
use std::io::Read;
fn read_username_from_file() -> Result<String, io::Error> {
    let mut s = String::new();
    File::open("hello.txt")?.read_to_string(&mut s)?;
    Ok(s)
}
```
上述代码中先`open`文件，如果`Result`是`Err`则向上抛，否则执行`read_to_string`如果`Result`是`Err`向上抛


对于`option`也可以使用`?`,如果`Option`是`Some`则返回里面的值，否则返回`None`
```rust
fn first(arr: &[i32]) -> Option<&i32> {
  let v = arr.get(0)?;
  Some(v)
}
```

### map 和 and_then
```rust
fn add_two(n_str: &str) -> Result<i32, ParseIntError> {
   n_str.parse::<i32>().map(|x| x+2)
}
```
上述代码中`map`为`Ok`或`Some`的值做修改，如果是`Err`或`None`则什么也不做.最后返回`Result`或`Option`

`and_then`用于链接多个可能返回`Option`或`Result`的操作，其中每一个都可能失败。

`map`和`and_then`的区别是
`map`的闭包中的返回值是`Result`或`Option`内包裹的类型。而`and_then`的闭包的返回值是一个`Result`或者`Option`类型的，因此`and_then`每个调用都可能导致`Err`或者`None`

`map`和`and_then`的区别是
`map`的闭包中的返回值是`Result`或`Option`内包裹的类型。而`and_then`的闭包的返回值是一个`Result`或者`Option`类型的，因此`and_then`每个调用都可能导致`Err`或者`None`


## 包和package
### 包crate
独立的可编译单元，编译后生成可执行文件或库

### package
`package`是一个项目,包含独立的`Cargo.toml`,一个`package`只能包含一个`lib`但是可以包含多个二进制可执行类型的包。

## module
`cargo new --lib restaurant`
创建库类型的`package`
```rust
// 餐厅前厅，用于吃饭
mod front_of_house {
    mod hosting {
        fn add_to_waitlist() {}

        fn seat_at_table() {}
    }
    mod serving {
        fn take_order() {}

        fn serve_order() {}

        fn take_payment() {}
    }
}
```
所有模块需要定义到一个文件中

模块的根是`crate`当需要访问模块内的成员时有两种方式
1. 绝对路径`crate::path1::path2`
2. 相对路径`path3::path4`

同一个模块下成员是相互可见的，但是子模块就不一定能被父模块同一作用域的其他成员访问。
```rust
mod front_of_house {
    mod hosting {
        fn add_to_waitlist() {}
    }
}

pub fn eat_at_restaurant() {
    // 绝对路径
    crate::front_of_house::hosting::add_to_waitlist();

    // 相对路径
    front_of_house::hosting::add_to_waitlist();
}
```
上述代码中`eat_at_restaurant`能访问`front_of_house`但是不能访问其子模块。

需要`pub`修饰子模块，同样的，子模块内的成员也需要`pub`才能被访问

在`rust`中，父模块无法访问子模块的私有项，但是子模块可以访问任意祖先模块的私有项


### super 和self 
在路径引用中，我们提到了根模块是`crate`
当我们想访问父模块中的成员时可以使用`super::`，`self`则是访问本模块的成员


### 结构体和枚举的可见性
结构体成员对于其他模块的可见性默认是私有的，即使结构体本身是`pub`的。
而枚举的字段可见性是跟枚举类型一致的

### 多文件编写
```rust
mod front_of_house;
pub use crate::front_of_house::hosting;
pub fn eat_at_restaurant() {
    // 绝对路径
    crate::front_of_house::hosting::add_to_waitlist();

    // 相对路径
    front_of_house::hosting::add_to_waitlist();
}
```
第一行`mod`表明要从`front_of_house.rs`读取该模块的内容。


当我们想要多文件多目录的管理我们的模块时可以按照下面的方式管理
```ascii
src
├── front_of_house
│   └── hosting.rs
├── front_of_house.rs
└── lib.rs
```
`hosting`编写本模块代码
`front_of_house.rs`内使用`pub mod hosting;`来加载`front_of_house`目录下的`hosting`模块


## use
`use std::abc::efg` 可以有效的减少代码量
由于模块之间的成员会存在同名冲突的情况，因此我们可以使用`as`来给一个别名
`use std::io::Result as IoResult`

我们可以对`use`进行导出`pub use`，这样可以直接将模块内部的成员暴露出来，提供给其他模块使用.


### 使用第三方包
```tmol
[dependencies]
rand = "0.8.3"
```
`rust-analyzer`会自动拉取该库


`use std::collections::{HashMap,BTreeMap}`
`use std::io::{self,Write}`
`use std::io::*`

### 受限的可见性

我们不仅可以控制哪些模块可见，还可以控制对谁可见。
```rust
pub mod a {
    pub const I: i32 = 3;

    fn semisecret(x: i32) -> i32 {
        use self::b::c::J;
        x + J
    }

    pub fn bar(z: i32) -> i32 {
        semisecret(I) * z
    }
    pub fn foo(y: i32) -> i32 {
        semisecret(I) + y
    }

    mod b {
        pub(in crate::a) mod c {
            pub(in crate::a) const J: i32 = 4;
        }
    }
}
```
在`semisecret`函数中，我们原本无法访问子模块`b`中的成员。通过`pub(in crate::a)`限制了只有在`a`模块内`b`是`pub`的，使得我们可以在函数中使用其内部的成员了

- `pub(crate)`
- `pub(self)`
- `pub(super)`
- `pub(in path)`:其中`path`必须是祖先模块


## 注释
和`c`一样，支持双斜杠和`/**/`的注释方式
文档行注释:
三斜杠或者`/** */`
文档行注释中可以编写`md`格式的文本

`cargo doc`会自动生成`html`

### 包注释
在`lib.rs`最上方`/*!   */`之间写关于包的注释

### 文档测试
我们在给方法写的文档注释中的代码是可以执行的，我们可以在此处编写单元测试。

对于那些要`panic`的测试我们可以添加`should_panic`到代码块开头


对于那些我们不想要被显示在文档上的代码，可以用`#`开头，这样这些代码在测试时还是会被执行，但是不会被显示到文档中


### 外部链接
```rust
/// `add_one` 返回一个[`Option`]类型
pub fn add_one(x: i32) -> Option<i32> {
    Some(x + 1)
}
```
其中的`option`在文档中可以直接点击跳转到对应的文档

```rust
use std::sync::mpsc::Receiver;

/// [`Receiver<T>`]   [`std::future`].
///
///  [`std::future::Future`] [`Self::recv()`].
pub struct AsyncReceiver<T> {
    sender: Receiver<T>,
}

impl<T> AsyncReceiver<T> {
    pub async fn recv() -> T {
        unimplemented!()
    }
}
```
在上述代码中还能看出我们可以使用不同的路径来进行跳转，支持`use`

### 同名项的跳转
```rust
/// 跳转到结构体  [`Foo`](struct@Foo)
pub struct Bar;

/// 跳转到同名函数 [`Foo`](fn@Foo)
pub struct Foo {}

/// 跳转到同名宏 [`foo!`]
pub fn Foo() {}

#[macro_export]
macro_rules! foo {
  () => {}
}
```
在后方进行标注类型可以支持同名项的跳转

### 文档搜索别名
```rust
#[doc(alias = "x")]
#[doc(alias = "big")]
pub struct BigX;

#[doc(alias("y", "big"))]
pub struct BigY;
```
在文档搜索时使用别名命中时搜索结果会被放在第一位

## 格式化输出
- `print!`:不换行
- `println!`:换行
- `format`:到字符串

### 具名参数
```rust
fn main() {
    println!("{argument}", argument = "test"); // => "test"
    println!("{name} {}", 1, name = 2); // => "2 1"
    println!("{a} {c} {b}", a = "a", b = 'b', c = 3); // => "a 3 b"
}
```

可以给给参数指定名称
### 位置参数
可以指定位置的参数去替换占位符
`println!("{0}, this is {1}. {1}, this is {0}", "Alice", "Bob");` 

带名称的参数要放在不带名称参数后面。

### 指定输出格式
```rust
// 为"x"后面填充空格，补齐宽度5
println!("Hello {:5}!", "x");
// 使用参数5来指定宽度
println!("Hello {:1$}!", "x", 5);
// 使用x作为占位符输出内容，同时使用5作为宽度
println!("Hello {1:0$}!", 5, "x");
// 使用有名称的参数作为宽度
println!("Hello {:width$}!", "x", width = 5);
```
其中`$`是使用参数的意思


```rust
fn main() {
    // 宽度是5 => Hello     5!
    println!("Hello {:5}!", 5);
    // 显式的输出正号 => Hello +5!
    println!("Hello {:+}!", 5);
    // 宽度5，使用0进行填充 => Hello 00005!
    println!("Hello {:05}!", 5);
    // 负号也要占用一位宽度 => Hello -0005!
    println!("Hello {:05}!", -5);
}
```
数字与字符串不同，数字默认是右对齐


对齐
```rust
// 以下全部都会补齐5个字符的长度
// 左对齐 => Hello x    !
println!("Hello {:<5}!", "x");
// 右对齐 => Hello     x!
println!("Hello {:>5}!", "x");
// 居中对齐 => Hello   x  !
println!("Hello {:^5}!", "x");

// 对齐并使用指定符号填充 => Hello x&&&&!
// 指定符号填充的前提条件是必须有对齐字符
println!("Hello {:&<5}!", "x");
```
指定符号填充的前提必须是显式写出对齐方式

进制
```rust
// 二进制 => 0b11011!
println!("{:#b}!", 27);
// 八进制 => 0o33!
println!("{:#o}!", 27);
// 十进制 => 27!
println!("{}!", 27);
// 小写十六进制 => 0x1b!
println!("{:#x}!", 27);
// 大写十六进制 => 0x1B!
println!("{:#X}!", 27);

// 不带前缀的十六进制 => 1b!
println!("{:x}!", 27);

// 使用0填充二进制，宽度为10 => 0b00011011!
println!("{:#010b}!", 27);
```
指针
```rust
let v= vec![1, 2, 3];
println!("{:p}", v.as_ptr()) // => 0x600002324050
```
转义
`{{`为`{`
`}}`为`}`
`\"`为`"`

# 进阶
## 编写测试

```rust
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }
}
```
测试函数需要使用`test`标注，这样就可以被测试器发现并运行。


如果我们要测试某个方法`panic`是正确的，需要对方法标注`#[should_panic]`,这样当方法`panic`时会显示测试通过，不`panic`测试失败


有时代码发生的`panic`和我们预期的`panic`可能也不一样，这时需要加上修饰
```rust
// --snip--
impl Guess {
    pub fn new(value: i32) -> Guess {
        if value < 1 {
            panic!(
                "Guess value must be greater than or equal to 1, got {}.",
                value
            );
        } else if value > 100 {
            panic!(
                "Guess value must be less than or equal to 100, got {}.",
                value
            );
        }

        Guess { value }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    #[should_panic(expected = "Guess value must be less than or equal to 100")]
    fn greater_than_100() {
        Guess::new(200);
    }
}
```
在标注时显式标注出期待的`panic`，只要标注出的期待的字符串是得到的前缀就算通过测试


当测试函数多了的时候，我们不可能每次修改完代码重新跑所有测试程序，这时我们可以指定执行某个测试函数`cargo test func`,但是这种方法只能指定一个


当我们想要忽略某个测试程序的时候，只需要标注出来就好了`#[ignore]`


`[dev-dependencies]`是`cargo.tmol`中只在开发测试使用的外部以来
我们可以引用`pretty_assertion = "1"`

### 单元测试
直接在`lib.rs`里面写的就是单元测试，只测试公开的方法。

### 集成测试
集成测试代码是放在单独的目录`tests`下的。
当我们的许多集成测试代码文件都需要同一个功能时，我们最好能够提取出来单独写一个模块。
需要在`test/common/mod.rs`编写共享模块。


### benchmark


# 进阶
## 生命周期
 
