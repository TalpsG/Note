# page fault
## book 4.6 pagefault

xv6对exception的响应是很boring的:
用户空间的exception会导致process被kill

kernel里的excep会导致kernel panics

现代操作系统都会响应的更interesting一点

许多内核使用pagefault 去实现copy on write的fork
在对子进程的地址空间进行写时才复制数据

fork使子进程有和父进程一样的内存内容(uvmcopy分配物理页并复制数据)
如果子进程和父进程共享父进程的物理内存，会更有效率
一个直接的实现方式并不可行，因为父进程和子进程之间并未隔离，他们的栈和堆是共享的

父子进程可以安全的共享物理内存通过cow fork(cow借助pagefault实现)
cpu找不到虚拟地址对应的物理地址时，会出现pagefault 也就是缺页异常
riscv有三种 pagefault: 
1. load page faults(load指令使用地址然后发现找不到对应的物理地址)
2. store page fault (store指
令使用地址)
3. instructions page fault (读取指令时的pagefault)

scause 值表示具体的缺页类型
stval 存放这不能翻译的地址

cow fork的基础就是让父子共享物理页，但是map的时候使用readonly的方式
因此当执行store指令，riscv会抛出pagefault
为了响应异常，可以复制对应的物理页，然后map上去就可以了
然后继续执行出异常的指令就可以了

cow 非常有用，因为fork后经常就跟着exec.
一般情况下子进程只需要几次pagefault就可以继续执行，而内核也不需要复制整个地址空间了
而且cowfork是透明的，没有对用户程序做修改


pagetable和pagefault的结合提供了很大的可能性，一个是cowfork
另外一个是lazyallocation:
1. 当程序使用sbrk时，kernel 增长用户地址，但是新地址 pte上的valid位为0,置为无效
2. 当出现pagefault的时候再去分配物理页，并写到pte里

因为用户申请的空间通常都大于要使用的，所以lazyalloc是一个很成功的策略
当用户真的使用这块区域才进行分配

另一个广泛使用的特性paging from disk
应用需要大于物理内存的空间的时候，kernel可以evict (驱逐)一些页到磁盘上，并且将pte的valid置为无效，应用使用这些页再通过pagefault 把磁盘上的page读进来，pte的valid改回来(但是又会evict其他的页)




## class
### basics
xv6并没有实现上述的那些特征，这节课的lab就是lazyallocation

虚拟内存的优点
1. isolation
2. 抽象:trampoline和guard page


xv6实现的pagetable是静态的，设置好就不在改变了
而pagefault可以让地址映射的关系变得动态起来

### lazy allocation
sbrk可以扩大程序的堆

在程序最开始处，sbrk指向的是栈底，然后sbrk可以向上扩大heap的空间，stack在heap下
当我们调用sbrk的时候，仅仅增加p->sz的大小，当程序访问对应内存出现pagefault的时候进行判断，如果虚拟地址大于stack但是却小于p->sz,则这个地址就是heap中的地址，为其分配page然后map上去就可以了

当出现store pagefault的时候 scause为15
可以在usertrap中判断scause找到store pagefault进行处理
需要注意的是，在lazyallocation中，unmap内存也要小心的进行
因为page是lazy 分配的 所以unmap的时候不可以unmap整个地址空间

### zero fill on demand
程序的bss段应该是全0,bss段可能占用很多页面
可以在程序开始运行时把bss段都映射到一个页上，只读
当要写入数据时，就通过pagefault分配页面然后初始化为0，最后map上去就可以

zerofill on demand类似lazy allocation

### copy on write fork
fork的时候会拷贝父进程的地址空间
而大部分情况下fork完会执行exec,并没有用上父进程地址空间，所以拷贝是无用的开销

我们可以让父子都对其地址空间对应的物理空间只读，然后进行写操作时由pagefault处理
对对应进程分配page并进行copy，最后进行写操作

但是在释放内存的时候需要小心，因为只读的页面不可以释放，而copy过的页面才可以释放
但是只读的页面如果没有进程使用也需要释放，如何解决(可以为cow复制的页面添加引用次数，但是开销不小)

### demanding paging
事实上在exec中会为程序分配页面并且读入程序的text 和data区域
我们也可以实现lazy alloc方式的加载，在用到对应的数据时再进行读入
我们可以吧pte的v设置为0,然后在pagefault里进行读取


### memory mapped file
mmap
将文件读入内存，加快文件的访问
lazy的方式进行读取
在mmap的时候先记录文件的信息
比如存放在vma结构体中
