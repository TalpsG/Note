# 3
## 3.7 sbrk代码
sbrk是进程控制自己内存的syscall
sbrk调用growproc实现，growproc调用了uvmalloc和uvmdeallc(分别是分配和释放)

进程的pagetable不光是告诉硬件如何映射地址，也是唯一的物理地址分配给进程的记录。所以当释放page的时候需要检查用户页表


## 3.8 exec代码
exec从文件(elf)中初始化了用户部分的地址空间
exec分配了一个新的页表，然后为eelf中的段分配内存，并加载这些段进入内存

比如init为例
exec先加载init的program section,然后分配栈空间
复制argc和argv到栈底(实际上是高地址处，因为栈是从上往下长的)
ustack存放argv指针，sp指向用户栈底，将argv字符串放入栈底，然后将ustack的指针也放入栈中。

之后放一个不能访问的页用来当哨兵，检测栈溢出。

最后要释放现在的页表然后使用新创建的页表。

## 3.10 练习

