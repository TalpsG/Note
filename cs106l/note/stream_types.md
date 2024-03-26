# stream
## stream


`stream`是有`openmode`的，比如
```cpp
int main(int argc, char *argv[]) {
  ostringstream oss("wangweitao");
  string s = "talps";
  oss << s << s << s;
  cout << oss.str() << "\n";
  return 0;
}
```
上述代码默认的`ostringstream`的模式，会在字符串流开头输入，前面的输入覆盖掉原来的字符串。
`ostringstream oss("wangweitao", ostringstream::ate); `指定`ate`模式后就会在原有字符串后面增加。


流会跳过空白符号比如换行`tab`空格等。

我们可以通过`tell` 等成员函数来获取和设置流当前读取的位置。


### statebit
流当中有一些成员变量是用来标识流的状态的。

`F`: 当流发生错误，比如`cin`读数据的时候类型错了
`G`: 没有错误
`E`: 流读完了
`B`: 流无法读取了，比如文件流的文件被删除了


### buffered
有些流是有`buffer`的，为了提高性能，不会每次都输出。
`cout`就是有`buffer`的，我们需要手动`flush`掉。

相关流的使用也会导致`buffer flush`，比如`cout`的`buffer`在使用`cin`时会被`flush`掉。


### 一些操作符 manipulator
`endl`:insert newline ,flush
`ws`:skip whitespace
`boolalpha`:print "true" or "false" for bool value
`hex`:print hex value
`setprecision`:set precision like width

`cout << "[" <<setw(10) <<"]";` 设置打印宽度
`setfill(char)` 宽度填充的字符.

## types and stream
`cin <<` 不好用的三个原因
1. 跳过空白
2. `cin`的`buffer`还有数据时，会使用`buffer`中的数据
3. `cin`出错后，所有之后的`cin`都会出错


`std::getline(istream,strbuf);`


- 对于用户输入的错误检查
  - 使用`getline`而非`cin`
  - 使用`istringstream`解析`line`
- 使用`statebit`控制流实现错误检查


### modern c++ types
- 使用别名
- `auto`会忽略`const`和引用


为什么使用`auto`
- 好用
- 灵活，修改源代码后`auto`不用改
- 在模板中很强大

### pair tuple

`auto price = make_pair(3.4,5);`会自动生成`pair<double,int>`的对象
`auto values = make_tuple(3,4,"hi");`同理，生成`tuple<int,int,char*>`


对`pair`我们使用`first`和`second`来获得其中的元素。
对`tuple`来说，我们用`get<index>(tuple1)`来获取元素.


在`c++17`中，我们也可以使用类似`python`的语法啦！
```cpp
auto price = make_pair(3.4,5);
auto values = make_tuple(3,4,"hi");
auto [a,b] = price;
const auto &[x,y,z] = values;
```


### struct
```cpp
struct Base {
  double d;
  int i;
  string s;
};
Base b{1.0, 2, "wwt"};
auto [d,i,s] = b;
```
对结构体也可以实现类似`tuple/pair`的解包方式


### reference
不要返回一个局部变量的引用


### conversion
`static_cast`:用于静态的不同数据类型的强转
`const_cast`:用于`const`之间的强转

