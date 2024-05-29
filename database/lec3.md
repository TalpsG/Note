# lec3 db storage p1

在内存中我们可以按字节寻址，可以进行随机访问。
在内存之下的存储结构中比如`ssd`或硬盘，我们是按块寻址。

`intel`曾经研发了一个叫`optane`的东西，实现了内存的高速字节寻址以及`ssd`的大容量持久存储的特点，但是被`intel`放弃了，因为成本问题。

## 顺序 vs 随机
`DBMS`会尝试减少随机页的访问，尽量让数据连续存放。
一次分配多个页面的技术叫做`extent`

## 系统设计的目标：
- 允许`DBMS`管理超过内存大小的数据库
- 小心的读写磁盘，防止性能下降以及出现卡顿现象

## 为什么不用OS
如果使用`mmap`去操纵内存，那么`dbms`不需要去管理内存了，内存会被`os`管理，`os`会移动页面，回收页面等等等等。

`os`的管理并不考虑`dbms`的操作，因此`os`对于内存的管理可能会导致`dbms`出现性能下降的现象。

1. 而且当有多个写操作时，可能会有顺序的要求，而`os`并不会主动处理顺序的要求。
2. `IO stall`由于读写导致的缺页会使得线程等待。
3. 由于使用`syscall`，当出现错误的时候会得到`sigbus`，你知道的，`c`的异常处理是很麻烦的。
4. 性能下降

`os`提供的上述问题的一些解决方法

- `madvise`，告诉`os`期望的内存访问方式(顺序，随机等)
- `mlock`，告诉`os`某些页面不能被换出
- `msync`，告诉`os`需要写回某些页面了

存在一些`kernel bypass`的方式直接操作硬件。
类似`DPDK`可以直接获取`TCP`的包

`dbms`可以比`os`做得更好
- 按顺序写回页面
- 特定的数据预读取
- 缓冲区替换策略
- 线程进程调度

## 问题1 
数据在磁盘上如何表示？
大多数数据库都是特定格式的文件存放在磁盘上的。

## 存储管理者
`storage manager`来管理数据库文件，以及管理读写相关操作，提高时空局部性。
这一个层次并不会为页面存放许多拷贝，这是其他层要做的事情。

有些系统的页面要求保存自身的元信息，以便处理存储错误或其它问题。

像`os`一样，页号是通过一个中间层进行管理的。
**页号实际上是数据在文件中的偏移量**
数据库的页面是不同于硬件页面和`os`页面的。
(硬件页面保证了对一个页面写入的原子性)
我们的数据库页面越大，越有利于读取操作，但同时，写入操作也需要一次性写入更多数据。

## 存储架构
- 堆文件
- 树文件
- 顺序文件，有序文件
- 哈希文件

### 堆文件
堆文件是页面的无序集合。
只需要提供简单的页面操作和迭代操作即可。

页面如何组织成堆文件？
1. 链表
2. 页面`directory`类似页表
  - 有一个特殊的页面记录了单个或多文件数据库的元数据，利用该页面使用页号来访问。任何对该页的修改都要写入磁盘，避免不一致的问题出现。

## 页面布局
页面会有一个`header`包含了一些元数据。
页面大小，校验和，数据库版本，事务可见性等等等等。

页面中的数据是如何存储的？
1. `tuple-oriented` 
2. `log-oriented`
3. `index-oriented`
后面两种之后的课程会讲
### tuple
如何实现`tuple-oriented`？
初步想法： 在文件末尾添加`tuple`,类似数组，每一条数据都是固定大小的。
缺点： 数组的缺点他都有，而且数据大小必须一样大.
当中间数据被删除时，移动其他数据需要修改所有追踪该页面的数据。

解决问题的方法是`slotted array`：
在页面的`header`中记录每一条数据的起始地址，这些地址组成一个数组。`tuple`是从页面的结尾处向前存放的，也靠近页面结尾的`tuple`是靠前的数据(正着插入数据需要知道上一条插入的数据大小)

当我们删除了数据并回收了对应空间后，我们只需要移动删除位置之后的`tuple`并且修改`slotted array`即可，不需要在修改额外的信息了。

### record id 
`dbms`通过`record id`来访问实际的`tuple`.
通常`record id`是`file id` ， `page id` 和`Slot num`的组合.
`tuple`并不存放`record id`
应用程序不应该依赖这些`id`因为他们`dbms`内部使用的，且会变化的(比如`garbage collection`之后)。

不同的数据库实现会不同，比如插入`tuple`时有的数据库会插入到空槽内. 而有些则会在末尾插入,在`gc`后才会移动`tuple`

## 总结
数据库是页面组织起来的。
跟踪页面的方式有许多。
存储页面的方式也有许多。
存储`tuple`的方式也有许多。