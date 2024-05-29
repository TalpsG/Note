# c++ 并发编程
这本书基于`c++17`及以前的标准.

## 第一章 你好，并发世界

### 1.4  入门
`thread t(func)`来创建线程
`t.detach`分离线程，主线程不再等待其他线程。
`t.join`主线程等待其他线程结束再结束。

如果需要更细粒度的控制，需要使用条件变量或是`future`等其他机制来实现。


在线程执行过程中可能会抛出异常，这样我们的写的`join`会无法被执行，需要小心的添加`trycatch`来保证线程内容的回收。


`RAII`的方式
```cpp
struct thread_guard{
  thread &t;
  ~thread_guard(){
    if(t.joinable()){
      t.join();
    }
  }
}
void f(){
  int local = 0;
  thread t(func);
  thread_guard g(t);
}
```
上述处理方式在`f`函数执行完后会回收`guard`，在其析构中会`join`从而等待以及回收线程，无论是否发生异常。

### detach
`detach`的进程会在后台运行，

## 第二章
### 线程
```c
func(){
  int temp = 10;
  std::thread t1([&](){
    for(int i=0;i<1000000;i++){
      temp += 1;  
    }
  });
  t1.detach();
}
```
上述代码中已经决定不等待`t1`后，func执行完毕，线程中的函数可能还在运行会导致访问已经销毁的变量

此处可以使用`join`等待线程完成，线程只能使用一次`join`函数，之后就不是`joinable`的了。

```cpp
void f(int i,std::string const& s); 
void oops(int some_param)
{ 
  char buffer[1024]; // 1 
  sprintf(buffer, "%i",some_param); 
  std::thread t(f,3,buffer); // 2 
  // std::thread t(f,3,string(buffer)); // 2 
  t.detach(); 
} 
```
上面的代码里，oops中的t由于已经分离了，所以可能会出现oops执行完毕了，线程的参数`char *`转换到`string`还没有完成。


线程要执行的函数如果需要左值引用的参数，在构造`thread`对象时需要用`std::ref`来将左值转换为引用.


### 线程移动
线程支持`std::move`来进行移动，不可拷贝。
移动赋值运算符会使得原线程对象的线程被`terminate`掉。

## 共享数据
### lock_guard mutex
即使是用了互斥量，也可能出现危险.比如通过某种手段将内部保护的数据指针传递了出去。
