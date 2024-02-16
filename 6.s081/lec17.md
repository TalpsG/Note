# virtual mem
## vm for user programs
用户程序也可以使用系统内和所使用的虚拟内存技术获益。

- 垃圾回收器
- 数据压缩程序
- 共享虚拟内存

## syscall for vm to user programs
mmap或许是最重要的,提供给用户程序使用虚拟内存的syscall。

mmap可以将文件映射到指定的内存地址中。

此外还需要一些syscall来做一些额外的工作
1. `mprotect`用于对虚存修改保护权限
2. `munmap`移除虚拟内存的映射
3. `sigaction`，用于设置进程对特定信号的响应。


## how to use vm for user programs

在操作系统层面,虚拟内存的实现是通过硬件地址翻译和一些数据结构实现的。这些数据结构被称为`virtual memory area`,记录了关于虚拟地址段的一些信息。

如果程序对一个文件使用了`mmap`映射到了内存中，则对应VMA会记录文件的读写权限，文件的一些元数据等。

如果发生`segfault`
1. segfault会被用户程序感知
2. 用户空间会调用对应而handler处理segfault
3. handler调用mprotect修改PTE
4. handler返回内和代码
5. 内核返回到原进程


## make bigger cache table

如果有一张表记录了费时函数的运行结果，那么调用这些函数的时候我们就可以去查表而非直接运行。但是这个表可能会很大，超过物理内存的大小了，此时我们需要虚拟内存来解决该问题。

首先分配一大段虚拟内存，但是并不映射到物理内存中。这段内存用于保存表单。
所以现在查表的时候会触发`pagefault`，然后在`pagefault`的时候进行函数调用，并将答案填到发生`pagefault`的内存地址处。

用户如何收到`pagefault`？
通过类似`signal`的函数挂载`handler`,之后在发生对应信号的时候就会调用`handler`进行处理

## use vm for gc
对`to`区域划分为`scanned`和`unscanned`,`scanned`区域中对象的指针都指向`to`，而另外的区域则不一定。
在用户程序只会访问到`to`区域，如果用户程序访问了`to`中的`unscanned`中的指针，则扫描指针所在的那一页，把页内的指针指向的对象都复制到`to`，这一页也就变成`scanned`。


## code : gc

1. 没有空余内存分配空间时,开始gc,否则分配在`to`空间
2. gc时,调换`to`和`from`，之后将`to`空间设置为不可访问的状态
3. 移动根节点到`to`空间
4. 此后应用程序访问两个根节点将会出现`pagefault`
5. `handler`内部扫描访问的页面，然后搬运对应对象，然后修改页面权限






