# crash safety

#  intro
像数据库一样，对文件的写入要原子的进行，当对有文件syscall的时候用begin和end包裹起来，形成一个transaction,事务是原子性的。

## logging
日志就是一种朴素的crash safety的保证.
对文件的写入是原子的，且是可以恢复的。

当要对文件更新的时候，我们先将内容写到log中，然后等几个文件更新操作之后一起写到磁盘里，这个就叫事务


```c
struct logheader{
    int n;
    uint block[N];
};
```
n代表着log的块数，block数组存放的是日志块对应的磁盘块号

## logwrite函数
logwrite实际上是bwrite的一层封装，所有对磁盘的写都会调用logwrite,写入日志块内，等到commit的时候才会从日志块写入磁盘块。

## 事务的提交过程
1. 把log块都写到磁盘的log块里
2. 把log头写到磁盘log头里
3. 提交事务，将磁盘的log块和要写的块都读出来，然后将log块写到目标块内，再调用bwrite写回磁盘，完成事务提交


## 文件系统的恢复


xv6 并不保证写盘块的原子性，也就是说commit里面写logheader到磁盘我们要假设他是原子的。


开机的时候进行恢复，当logheader的n>0的时候就说明log里面有数据，就需要恢复到磁盘。

## 文件系统的挑战
1. cache驱逐，日志的块是不可以被驱逐出去的，因为驱逐出去就要写回磁盘，但是这不符合事务的原子性
2. 文件系统的操作必须适配日志的大小，对文件系统的操作必须小于日志空间。由于日志块的cache要在写入磁盘之前留在buffer中，所以buffercache的块数要比log块多

