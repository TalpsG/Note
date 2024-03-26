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
