# lec1 关系模型
关系模型是独立于查询语言实现的。

# lec2 sql
sql包含了三种语言的功能
1. dml 数据操作语言
2. ddl 数据定义语言
3. dcl 数据控制语言 访问权限等


## 语法
基本语法:
`select name ,gpa from students where age >25;`

## 聚集函数
一些常用函数，比如`count`,`max`,`min`等。
`select COUNT(*) as cnt from student where login LIKE "%@cs" `

`COUNT AVG SUM`支持`DISTINCT`会先去重，在进行函数运算。

`GROUP BY`:
`select AVG(s.gpa),e.cid 
from enrolled AS e,student AS s
where e.sid = s.sid
GROUP BY e.cid`
上述语句会按照`e.cid`进行分组，之后运算平均值，然后输出每一个组的平均值。

`HAVING`：
在`GROUP BY`后查询的结果，想要继续处理要使用`HAVING`

`CREATE TABLE tname (select )`

`LIMIT`：限制输出的行数


`ALL ANY IN EXISTS`:
`ALL`是必须每一行都相同
`ANY`有一行就行，等价于`IN`
`EXISTS`只要查询有结果就为真
`selecto from  where a = ALL(inner query)`



## 结论
可以执行复杂的sql语句，让数据库系统自动优化。

## sql hw
字符串拼接: `column1 || "asdas" as new_column`
对某个列的`NULL`或其他特殊值进行特殊处理: `CASE when column ISNULL THEN 2023 else column END`

`CTE`：可以生成公共表达式，供下面的查找语句使用。
`DENSE_RANK`和`ROW_NUMBER` 都可以实现查找某个类别的固定个数的元素。

`Inner Join`
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
 
# 数据存储


`tuple-oriented` 的方式会浪费一些空间，页面当中会存在一些细小的碎片。
`useless io` 更新一个`tuple`需要读取整个页面
随机的`io` 一次更新多个不同页面的`tuple`会导致需要读取不同的页面然后写回

### log-structured
日志结构的存储实际上存储的是对`tuple`的修改。每一条日志都代表了一个对`tuple`的修改操作。

日志会按顺序存放在`buffer`里，满了之后写回到数据库中，有点类似`xv6`文件系统的日志层。

**日志写回到磁盘，但是还没有按照日志修改对应的磁盘数据。也类似`xv6`，在某个时间点会按照日志更新数据库数据**

老师说写回的日志访问的都是一个页面上的数据，我个人感觉是一个页面一个日志`buffer`？

日志结构使得写操作的速度变快了(因为写操作满`buffer`才会执行)

读取会变慢一些：我们从日志页倒着查找，如果有对应数据修改的日志，那么就直接查到了。否则需要磁盘读取。

加索引的方式可以改善读取的问题：
`tuple id`有指向本修改了数据`tuple`的日志或存放在磁盘的位置。


日志可以进行周期性的压缩，我们只需要保留相同数据最新的日志条目即可。对于删除的日志，我们可以删除对应`tuple`的索引，这样也就不需要存储删除的日志了。

日志页压缩过后，只剩下了`put`操作，每一个`tuple id`只会出现一次.
因此，我们不关心日志条目的顺序，只需要按`id`排序就可以了，这样会让我们的查找速度加快。

### log的问题
1. 更新一条数据可能伴随着多次的磁盘读写.
2. 压缩日志的代价不小

## index-organized
存储索引用来根据`record id`获取数据

索引可能类似树的形状，在叶子节点存放实际数据页面(`slotted page`)

索引树是有序的，因此查找的效率较高

## tuple layout
一条数据会有一个`header`用来记录比如每个字段的长度之类的信息。

类似结构体对齐，`tuple`数据也要存在对齐。
可以使用类似结构体的方式，填充数据来对齐8Byte,也可以进行重排数据来满足对齐的要求。


### 数据类型
数据库中的浮点数整数和`c++`的一样，依靠`ISA`的支持可以加快数据的处理速度。
但是机器支持的数据精度优先，比如`IEEE-754`的浮点数。

因此数据库会实现定点数来保证数据正确性


`NULL`:
- `tuple`的`header`存放一个位图来表示哪个字段是`NULL`
- 指定一个特殊值当作`NULL`
- 在每一个属性值前存一个`flag`指示这个值是否为`NULL`(浪费空间)


对于很大的数据类型(`varchar`)，数据不会直接存放在`tuple`页面内,而是存放其指针。

数据本身会存放在单独的页面中(`overflow page`)
可以采用链表的方式存放大型数据，`tuple`内的指针指向第一个溢出页，每一个溢出页都有指向下一个页面的指针

### 外部存储
有些数据库可以存放外部文件，但是数据库仅仅做存取，无法修改文件。

# lec5 
## database workload
1. `on-line transaction processing`:每次只读取和更新小部分数据,操作很快
2. `olap`: 复杂的查询
3. 混合


## 物理存储模型
1. `N-ary storage model`
  - 对`oltp`很好，因为操作(`select *`)通常伴随着访问某条数据
  - 对于复杂查询，常常会读取许多不需要的数据
  - 对于扫描大量数据或小部分属性的操作不太友好
  - 内存局部性不好(因为常常查询只访问部分属性)
  - 不容易压缩，因为是`tuple-oriented`
2. `decoposition storage model`
  - 一个页面只存储一个属性值,列存储
  - `olap`友好 
  - 好压缩
  - 如何判断值属于哪一行？
    1. 固定长度值可以使用偏移来计算行号
    2. 同时存储值和行号(legacy)
  - 对于插入更新删除来说是比较慢的
如何将变长数据变为定长？
## 数据压缩
1. 压缩结果定长
2. 尽量推迟解压
3. 无损


四种压缩方式
1. 块压缩:压缩几条`tuple`
2. `tuple`压缩
3. 压缩`tuple`中的较大的属性值
4. 压缩列中的部分值(通常是常出现的)

对数据库进行查询时`dbms`会将目标值也压缩一边方便查询。
### run-length encoding
通常会对数据排序，然后单独列一个表，表中按顺序存放值以及值所在行号的范围.

### bit-packing encoding
对于属性值大小总是小于属性类型的情况，压缩其类型，存放时以较小类型存放。
此时属性中个别较大的值会单独存放在另一张表内。


### bitmap
当属性总是几个固定值时，可以使用`bitmap`，每一个可能的属性值一个`bitmap`，属性值对应的`map`中的1代表对应行号的属性值为该值。


### delta encoding
对数据进行增量的记录。
比如一张记录时间和气温的表。
时间的这一列每一行都递增1分钟，气温的变化也不大。此时可以对除第一行以外的所有行进行增量记录，记录属性与上一条属性的差。

### dictionary compression
构造一种数据结构，其中将属性值和固定长度的整数值映射到一起。

字典的常见实现:
1. 数组，缺点很多
2. 哈希表：压缩和解压快，但是无法进行范围查找
3. `B+`树，可以进行范围查找，速度比哈希表慢


# lec6 
`dbms`如何移动磁盘和内存上的数据？

## buffer pool manager
缓冲池，池内有`frame`也就是`buffer cache`。
`latch`更底层，`lock`更上层
缓冲池可以有多个，因为多处理器使用一个缓冲池会因为锁而导致性能下降严重

### 数据预取
1. 在顺序扫描页面时可以预取页面
2. 在范围查找数据时，可以预取物理页面不相邻的数据库逻辑页
  - 比如b+树的相邻叶子节点属性值相近，但是并不是物理相邻的页面

### 共享扫描
比如
- `select SUM(val) from A`
- `select AVG(val) from A`
第二条指令执行时可以与第一条一起进行扫描(第一条指令已经扫描了部分数据)，第一条扫描完成后第二条指令继续从头扫描到自己开始的位置。

### buffer pool bypass
如果大批量的顺序读写，可能不需要缓冲池也可以获得较好的性能.

**`dbms`不使用os的`io`，因为不可控。**
## 分配策略


# lec7
如何读取页内的数据？
1. 哈希表
2. 树

## hashtable
1. 线性探测法哈希:静态表，无法扩容。删除需要特殊处理
  - 墓碑机制:标识一个特殊的值代表此处删除过其他值
  - 整理：删除元素后整理表。
2. `robin hood`： 元素中存放自己现在为止和应该位置的差，当冲突时，还是按照线性探测的方式寻找位置，寻找时比较元素的差值，大的不动，小的往后挪.
3. `cuckoo`：两个哈希表

# lec8
b+树索引
数据库中常常使用索引来加快查找速度，由于b+树特殊的性质，常常使用b+树来构建索引。

索引就是表中部分属性的副本。


## 节点
b+树的节点中存在一组`kv`,`key`是表中的属性值。
对于叶子节点来说`v`可以是`rowid`或者整个`tuple`，对于非叶子则是一个指针，只想下一层的节点。

这里有两种存储值的方式
1. 存储对应记录的位置，比如`rowid`等内部记录用的编号或指针。
2. 存储整个`tuple`

## 重复key
1. 将记录id和key一起当作key.
2. 允许叶子节点有一个`overflow`节点用来存放重复的key


## 聚簇索引
调整数据存放的顺序，使其和索引的顺序一样。
常用于主键上，对主键使用聚簇索引，这样主键顺序和数据存放在磁盘上的数据一样。


## 合并阈值
有时推迟合并会是一件好事，比如删除元素后紧跟着插入，此时可能会导致刚合并的节点又要分裂。

## 变长key
1. 指针
2. 变长节点，少见
3. 填充，实际上就是让每个key占用很大的足够存放key的空间。
4. 类似`slot array`

## 内部节点的搜索
1. 线性查找
2. 二分
3. 推测


## 优化
1. 前缀压缩:对于叶子节点中，有相同前缀的`key`可以记录一个共同的前缀和每个key不同的部分。对于内部节点则只记录不同的部分即可
2. `pointer swizzling`:当非叶子节点内部的指针指向叶子节点(即数据页面)在`buffer pool`时，可以直接让指针指向内存中的元素。
3. `bulk insert`:对插入的元素进行排序，然后插入，速度会快很多。

# lec9 
索引并发控制

- 逻辑正确性：线程看到的数据是他应该看到的么？
- 物理正确性：数据在硬盘上存放的正确性


`lock`偏向上层概念，保护的是抽象的数据。
`latch`是底层概念，不能回滚


## latch 实现
1. 使用os的`mutex`，不过效率不高因为有睡眠和唤醒的操作。
2. 读写锁:`shared_lock`,实现在自旋锁之上。


## 哈希表并发控制
1. 对页上锁
2. 对`slot`上锁
## b+树并发控制
`crabbing/coupling`的上锁方式:
对父节点上锁后对子节点上锁，如果父节点是安全的，则释放父节点的所，然后继续向子节点的子节点要锁，一直向下。


读取时就释放父节点锁，然后要子节点锁就可以。
写时需要一直持有祖先节点的锁，直到确定写节点时不会修改祖先的节点后，才释放所有祖先节点的锁


上述情况下上的锁都是写锁，会阻塞全部的读请求。不过大部分写操作实际上并不会影响大部分的祖先节点，因此我们还有一种乐观的选择。

与上面相同，但是在向下查找写入位置的时候对祖先上的锁是读锁，只有对最后修改的节点上写锁，此时如果不影响祖先节点，自下而上释放锁即可。
否则需要从根节点从新上一边写锁。


# lec10 sorting & aggregate
主要讲了外部排序。
## top-k heap sort 
如果一个查询包含了`order by`和`limit`，则使用`top-k`的堆排序是很有效的，如果数据都在内存中则，堆排序对这种需求很好

## 外部归并
外部归并排序分为两个阶段
1. 排序
2. 合并

对于待排数据`kv`，`value`可以是整个`tuple`也可以是`rowid`之类的指针

大致思想如下：
如果内存可以存放三个页面，则每次读入两个页面到内存，然后排序借助空页面将读入的两个页面排序输出回磁盘，继续读入未排序的页面。一轮过后，数据变成了有序页面的块，每个块中有两个页面。然后读取两个块的第一个页面，继续排序。此轮完成获得四个有序页面的块，以此类推，完成排序。


如果要排序的条件上有聚簇索引，则可以使用聚簇索引,这样就不需要排序操作了(聚簇索引会导致磁盘的数据按索引顺序存储)

## 聚合操作
`dbms`为了高效的完成聚合操作，需要一种快速的方式来对数据进行分组。
1. 排序
2. 哈希

对排序后的数据，去重或分组会更快。

但是排序的开销很大，因此我们往往不会因为聚集操作而对数据进行排序。

哈希在某些情况下的效果也很好
1. 只需要去重的时候
2. 计算量比排序低的时候


### 外部哈希聚集
1. 划分：将数据划分到`bucket`中
2. `rehash`：再次哈希，为了去重或`group by`之后的聚集操作


在`rehash`阶段中，在`hashtable`中存储`(groupkey,runningval)`的二元组，每次对一个`tuple`进行`rehash`，查找该表的`groupkey`,能对上就要修改`runningval`，否则创建新的二元组。
