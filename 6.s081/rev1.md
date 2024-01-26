# 3
## 3.7 sbrk代码
sbrk是进程控制自己内存的syscall ,sbrk调用growproc实现，growproc调用了uvmalloc和uvmdeallc(分别是分配和释放)

进程的pagetable不光是告诉硬件如何映射地址，也是唯一的物理地址分配给进程的记录。所以当释放page的时候需要检查用户页表


## 3.8 exec代码
exec从文件(elf)中初始化了用户部分的地址空间,exec分配了一个新的页表，然后为eelf中的段分配内存，并加载这些段进入内存

比如init为例:
exec先加载init的program section,然后分配栈空间.复制argc和argv到栈底(实际上是高地址处，因为栈是从上往下长的),ustack存放argv指针，sp指向用户栈底，将argv字符串放入栈底，然后将ustack的指针也放入栈中。之后放一个不能访问的页用来当哨兵，检测栈溢出。 最后要释放现在的页表然后使用新创建的页表。


# 4 trap和syscall
有三种情况可以导致cpu放下当前的指令执行，强制转移到一段特殊代码去处理事件

1. syscall:通过ecall指令
2. exception:比如除0
3. interrupt:比如读写请求，或读写完成

trap泛指三种情况。
通常的trap时无论执行什么代码，之后都要resume,cpu不需要知道发生了什么事。
被中断的代码不希望发生中断。

xv6 kernel 处理所有的trap
xv6 trap handling分为4步:
1. 硬件发出响应
2. vector(指向的kernel准备的代码)
3. 处理trap的代码
4. 细分trap后的处理代码(比如syscall或者driver)


## 4.1 trap机制
stvec: kernel trap handler的代码位置
sepc: 发生trap时的pc值
scause: trap号 **硬件自动填写**
sscratch: kernel设置了一个值,在trap handler会用上
sstatus: 状态寄存器，其中SIE位用于控制中断开关，SPP表示trap来自S还是U模式

上述的寄存器与s模式的 trap handle有关，不可以在u模式下被读写
还有一组m寄存器，与上述寄存器类似，只在定时中断这类特殊的情况下使用

每个core都有自己的一组csr，保证都可以处理trap

强制进入trap
1. trap是interrupt 则 clear SIE,不需要做下面的事情
2. clear SIE
3. sepc = pc
4. 保存mode 在SPP中
5. scause 赋值
6. 设置mode 为S
7. pc = stvec
8. 从pc处执行

注意cpu不会去切换pagetable 和stack,也不会保存任何reg。
这些需要kernel内的程序完成。

cpu只做最少的事情可以保留程序的灵活性

## 4.2 user space trap
从user code trap比从kernel里有挑战，因为satp指向的是user pagetable

因为rv硬件在trap中不切换pagetable,所以user pgbl必须有uservec的映射，也就是stvec指向的代码。
pgbl的切换由uservec代码完成，将userpagetable切换为kernel的，这样才能在切换后继续执行指令。
uservec在kernelpagetable和user中映射的位置是一样的，也就是trampoline


### uservec代码
uservec开始时，由于需要计算寄存器被保存的地址，和satp要被设置的值，
所以将a0与sscratch交换，a0被保存，且被赋值为sscatch原来的值:trapframe

在进入userspace前，kernel为每一个进程分配了trapframe用来保存trap时的gpr
trapframe就存放在sscratch中(trapframe存放在userpage中,虚拟地址位置就在trampoline下面)

trapframe还存放有kernel stack的栈指针，usertrap的地址，以及hartid
uservec取到这些值，切换pgbl然后跳到usertrap处

uservec大致操作就是保存gpr到trapframe,然后换pgtbl，跳转到usertrap内

### usertrap
usertrap的作用就是确定trap的原因，然后处理。

修改stvec，使之能够处理kerneltrap.

xv6的usertrap只有syscall或者device两种可能，其他情况会被kill掉进程


返回userspace需要执行usertrapret

### usertrapret
usertrapret设置了trapframe里的kstack栈指针，uservec的地址，hartid还有内核pgtbl
供下一次trap进来使用

最后调用userret函数

### userret
userret大致就是uservec的逆操作，先换pgble,然后取出pageframe的gpr,最后sret到sepc处





## 4.3 syscall
syscall的大致流程
1. 设置a7 为syscall的编号，a0,a1为syscall的参数
2. ecall 进trap
3. uservec, 换pgtbl,保存gpr,最后跳入usertrap
4. usertrap,中途调用syscall函数进行处理
5. syscall 函数，参数在trapframe里找
6. 返回usertrap,最后调用usertrapret
7. usertrapret 设置trapframe,最后调用userret
8. userret 换回pgtble,恢复gpr,最后sret跳回sepc处完成syscall的调用


## 4.5 traps from kernel space
kernel trap不需要在换pgtbl了，但是在kernel里首先要把stvec指向kernelvec处

kernelvec先保存了gpr在栈上，这很重要因为gpr的值是属于该线程的，如果trap导致切换了线程，则trap会返回到新的线程上，原来的线程数据会被保存在栈上。


kerneltrap只有两种类型，一种是device interrupt,还有一种是exception
不是device就要panic掉

device如果是2代表是timerinterrupt,yield切换线程
yield返回后，epc和sstatus可能被其他线程使用了，所以要恢复一下这两个寄存器
然后返回kernelvec，kernelvec恢复gpr 执行sret返回被被中断的kernel code处继续执行


# 5 中断
驱动是os管理特定设备的代码
1. 配置设备
2. 处理中断
3. 与进程交互

驱动代码可以很tricky,因为驱动的代码会被并发的执行,此外驱动必须了解设备的硬件接口
设备通常会发出中断，内核处理代码认出中断后会执行对应的handler，在xv6里是devinstr这段代码负责设备的中断


许多设备驱动执行在两个上下文中，上半部分在内核线程，下半部分在中断中
上半部分通常借助syscall，比如read write,这些代码会让设备开始工作，然后等待设备执行结束.

最终设备完成操作，然后raise interrupt 驱动的中断处理函数也就是驱动的下半部分识别什么设备完成了操作，并唤醒对应进程。如果需要也会告诉硬件执行正在等待的其他操作

## 5.1 终端输入
console通过uart接受数据. 用户进程比如shell 通过read读取用户输入到console的数据.uart的芯片是16550 ，是通过qemu模拟的。 在真机上16550会管理rs232串行总线, 在qemu里，16550连接键盘和显示

uart在软件中是以内存映射控制寄存器出现的，这里有一些物理地址连接着uart。 所以在访问这些地址的时候，不会访问对应的物理内存，而是直接访问设备。
uart对应的物理地址是0x10000000.这个地址处对应了几个 1byte宽度的 uart控制寄存器 
比如LSR寄存器存放的位表示输入数据是否正在等待被软件读取，这些数据可以从RHR中读取。每读一个uart就从他的内部的fifo字符中删除一个，在fifo空的时候清除lsr中的readybit.uart传输硬件很大程度上独立于接受硬件。如果软件向THR写入了一个字节，UART会传输这个字节

xv6 main调用了consoleinit 初始化了uart。配置了uart，使得uart在接受到数据后会生成读取中断，也会在发送一个字节输出后生成transmit 完成中断。

xv6 shell从console中读取。read会调用consoleread，consoleread等待输入到来并存入cons.buf中，并将其拷贝到userspace，当输入已经有一行的时候返回到用户进程。如果用户没有输入一整行，则读取进程会在sleep中等待。

当用户输入了一个字符，uart会向cpu发送一个中断，使得xv6进入traphandler。traphandler调用devintr，查看scause寄存器来判断是哪个外部设备发出了中断，如果是uart,就调用uartintr

uartintr读取全部uart中等待输入的字符并把他们交给consoleintr。这个过程不会等待任何字符，因为新来的字符会生成新的中断.consoleintr的工作是将输入字符存放到cons.buf,直到一整行输入都到达。当来了新的一行，consoleintr会唤醒等待的consoleread.
consoleread会观察cons.buf中的一整行数据，并copy到用户空间，然后返回到用户空间


## 5.2 console out
write syscall连接着console 最终到大uartputc。设备驱动保留着输出的buf,所以写进程不需要等待uart完成输出。相反uartputc会把字符添加到buf中，调用uartstart去开始设备的传输，然后返回。只有当buf已经满了才会让uartputc等待

每次uart发送完一个字节都会生成一个中断，uartintr调用uartstart确认设备真的完成了发送，然后给设备下一个要发送的数据。因此如果进程向console写入多个字节，第一个字节会是uartputc调用uartstart发送的，后面的字节都是传输完成中断uartintr调用uartstart实现的

通过buffering和interrupts可以使设备和进程解偶。console驱动可以处理输入，即使没有进程等待读取，接下来的read也可以看到输入。同样的，进程可以不需要等待地进行输出。这种解偶可以提高性能，通过允许进程并发执行设备io。在设备很慢或者需要立刻处理的情况下这种方式很重要。这个方法有时叫做并发io

## 5.3 并发驱动

你可能已经注意到了acquire了，acquire是上锁，为了保护consoledriver的数据结构不被并发访问。这里有三种并发的危险:
1. 两个进程同时调用consoleread
2. 硬件让cpu发送一个中断，但是这个cpu已经在consoleread里执行了
3. 在consoleread执行过程中，硬件可能给不同的cpu发送了中断请求

另一个在并发中需要注意的事情是一个进程可能等待输入，但是中断到达了其他的进程。
因此中断处理程序不允许考虑被打断的进程和代码。比如，一个中断handler不能安全的使用当前进程的页表调用copyout. 中断handler通常只做一小部分工作，然后让上半部分代码(read write等)去做剩下的部分

## 5.4 时钟中断
xv6 使用时钟中断切换计算密集的进程，yield 调用usertrap和kerneltrap 导致切换进程。时钟中断来自riscvcpu上的硬件。
riscv需要时钟中断进入m模式，riscv的m模式没有分页，带有一些csr,所以实际上在m模式里不能运行普通的xv6kernel代码。因此，xv6处理时钟中断完全的被上述机制分开了

start.c 里在main之前的代码，设置了接收timer中断。部分工作是设置clint硬件(该cpu核的interruptor)在固定延迟后生成中断。另外一部分工作是设置scratch区域(类似usertrap的sscratch),帮助时钟中断handler保存寄存器和clint寄存器的地址。最后start设置了mtvec和timervec开启了定时中断

timer中断可以发生在用户或kernel代码执行的任何时间。kernel在关键的操作中没法关闭定时中断。因此定时中断handler必须以某种保证不会打扰中断的内核代码的方式关闭定时中断。最基本的策略就是timer interrupt handler让riscvcpu 产生一个软件中断然后立马返回。riscvcpu 传递这个软中断给kernel,然后允许kernel 关闭它.（此处的意思大概就是用定时中断会让cpu发出软中断，这样kernel就有能力关闭或打开这个中断了）.处理这个软中断的代码在devintr中

m模式的mtvec指向timervec，保存了部分寄存器在scratch区域，然后告诉clint生成下一个定时中断，并告诉cpu生成软中断，恢复寄存器，然后返回。

timer interrupt handler代码全是汇编


## 真实世界
xv6 在kernel中允许设备和定时中断 ， 同样在用户程序中也允许他们。 timer中断强制切换线程，即使线程运行在kernel中。公平的分配时间片对内核线程来讲是很有用的，如果内核线程花了大量时间在运算而不返回用户空间的话。然而，内核线程需要留意他可能会被咱听到内核中，然后被恢复到不同的cpu上，这个也是xv6复杂性的来源。kernel可以被设计的更简单，如果设备和定时中断只发生在用户模式。
支持所有设备在一个典型计算机上是一件很重的工作，因为有太多的设备了，设备有太多的特性，在设备和驱动间的协议也很复杂，且文档也很少。所以在许多os里，驱动代码比内核代码多得多。

uart驱动一次取一个字节数据，通过读取uart 控制寄存器。这种方式被称作，程序化io.
因为软件驱动数据的走向。程序化io很简单，但是很慢。设备需要高速移动大量数据通常会使用dma。dma设备硬件直接将到来的数据写入内存，或从内存中读取数据。现代磁盘和网络设备都使用dma.dma设备的驱动会在内存准备数据，然后一次写控制寄存器去告诉设备处理准备好的数据就可以了。

中断很重要，当设备需要在无法预知的时间被关注，但是又不频繁的时候。但是中断的cpu开销很大。因此高速设备比如网络和磁盘控制器，使用了一些技巧减少中断的需求。一个技巧就是一次中断响应一批到来或者向外请求. 另外一个技巧就是关闭中断，然后周期性的查询设备。这个技术被叫做polling. polling在设备速度很快的时候很有效，但是他浪费了cpu的时间如果设备总是闲。有些驱动会动态的在polling和中断中切换。
uart驱动复制了到来的数据到kernel的buffer,之后到userspace。在低速情况下很有效，但是两次复制会降低性能，有些os支持直接复制数据到userspace,with dma.


# 6 locking
大多数kernel都在多任务执行间进行隔离。
由于multi core multi thread等等原因，我们需要隔离。
kernel充满了并发访问的数据，设计者需要花费更多的经历保证内核的正确性。

本章关注一个广泛使用的技术:lock
锁虽然可以提供隔离性，但也让许多操作变成串行执行了

## 6.1 竞争条件 race condition
想象一下两个进程在两个cpu上调用wait,则两个cpu都会调用kfree去freepages,如果不加锁可能会出现错误，page挂不到freelist上。

不加锁的实现方式在没有并发的情况下是正确的。但是如果并发执行，两个进程同时把自己的freepage 挂到freelist上，那么最后结果可能是错误的。

### race condition 
当一个数据被并发访问且至少有一个进程要写该数据，那么就发生了竞争。
**并发的调试是很困难的，比如打印调试信息可能会影响到共享资源访问的时间**

通用方法是锁，锁保证了互斥条件，所以可以只让一个cpu执行敏感的代码。

lock保护的实际上是数据的invariant。某些操作可能暂时违反invariants,但是必须在之后恢复。一个进程所需的数据可能被另外的进程修改过了

lock就是一种串行化的方式,锁让critical section内的操作变成了原子的。

## 6.2 code : locks
xv6有两种锁:spinlock和sleeplock

### spinlock
spinlock是一个结构体，内部有个locked变量，没上锁为0,上锁了为非0。
不幸的是，acquire这个函数的实现并没有保证互斥，所以可能会导致两个cpu上的进程都上锁的情况，我们必须保证上锁的行为也是原子的

riscv也提供了这样的指令，`amoswap r,a`,amoswap 交换a地址处和r寄存器的值。这条指令原子的先读取a地址数据，然后把r写入a,最后把读取到的a处的数据写入r。

xv6 acquire使用portable c lib中的`__sync_lock_test_and_set`函数，这个函数使用了amoswap指令，返回了旧的locked值.

acquire 函数 在loop里 要锁。每次循环交换1进入locked变量，然后检查上次的值，如果之前locked是0,则取到锁。如果之前locked是1，则写入1 locked的值也没有变化

一旦锁被获取，acquire会记录谁拿的锁，lk->cpu记录着这个值，它也被保护起来了，只能被拿到锁的修改。

release 与acquire相反，他清除lk->cpu的值，然后释放锁，同样的release也使用了portable c lib中的函数实现了原子释放锁的操作。


## 6.3 using locks
xv6在kalloc和kfree时使用了锁.

一个很难的地方就是确定要使用多少锁。有几个原则
1. 同一个变量同一时间只能有一个cpu写
2. 锁保护invariant,一把锁保护一个invariant

这些规则讲了什么时候锁是必要的，但是没说什么时候锁是不必要的。对于性能来讲不加锁是很重要的，因为锁会减少并发性。如果并发性不重要，那么可以只用一个线程。
一个简单的kernel可以设计成只有一把锁，对kernel进行上锁保护，很多单线程os就使用这个方法跑在多处理器上的，被叫做"big kernel lock".

但是这个方法牺牲了并行性:只有一个cpu可以在内核执行。
如果在内核中做了大量计算，更好的方法是使用大量的细粒度的锁。
作为粗粒度的例子，xv6的kalloc用锁保护了freelist


## 6.4 deadlock and lock ording
如果一段代码路径进入kernel必须持有几把锁，那么所有代码的执行路径都应该按照同样的顺序持有锁。反之则可能出现死锁。
比如t1 获取锁的顺序是ab,t2是ba,则t1要到a,t2要到b后，t1t2都会因为要不到锁而阻塞。所以所有代码获取锁的顺序要一样。

由于xv6的sleep的工作方式，xv6有许多长度为2的锁顺序链。比如consoleintr用来处理输入的字符，当新行到来，所有等待console输入的进程都会被唤醒。为了做到这样，consoleintr 在wakeup的时候持有cons.lock ，wakeup里为了唤醒进程会获取等待进程的锁.

全局死锁避免的锁顺序包括:在获取进程锁前必须先获取cons.lock

文件系统代码有着xv6最长的锁顺序:
创建一个文件需要同时获得目录的锁，和新的inode的锁，disk block buffer的锁，disk driver的vdisk\_lock 和 调用进程的锁。为了避免死锁，文件系统总是要按照上述顺序获取锁。

又是要取谁的锁事先不知道，因为一个锁必须持有后才能知道下一个要获取的锁是哪个。这种情况在文件系统查找路径下的连续component和wait和exit中(查找子进程)常常出现。最终，死锁的危险限制了锁的粒度，锁越多越危险。

## 6.5 locks 和 中断处理者
spinlock可能被线程和中断处理程序都使用，比如clockintr会增加ticks,而sleep也会读取ticks的值

spinlock的交互和中断产生了一个潜在的危险，如果sleep在获取锁后被clockintr中断了，然后中断中又要获取这个锁，就产生了死锁。
为了避免这样的情况spin如果被中断使用，cpu只有在关中断的情况下才能得到这个锁。
xv6更保守一点，当cpu获得任意锁，xv6必须关掉该cpu的中断，在释放锁的时候打开中断。这样即使在多处理器下，sleep获取锁后也不会被打断，会执行完毕直到释放锁。


## 6.6 指令和内存顺序
编译器经常会重排序指令，因为有些指令并没有相关性，重排列可以有效的提高cpu利用率。但是在重排序会改变并发代码的结果。cpu顺序规则被乘坐内存模型memory model.

xv6使用了`__sync_synchronize`去告诉编译器和cpu不要重排序load和store指令

## 6.7 sleep locks
spinlock会降低性能，且获得锁后，其他cpu获取该锁的时候会空转，且无法释放cpu.
sleeplocks可以在获取锁失败后sleep,等待释放锁

## 真实世界


# 7 调度

os都可能运行在多cpu的电脑上，所以需要分时共享cpu。理想的共享对用户进程应该是透明。一个通用的方法是为进程提供一个假象，让进程通过多路复用技术使用cpu。

## 多路复用
xv6通过切换进程实现多路复用。

首先xv6 的sleep和wakeup机制会在进程等待设备或者管道io，等待子进程推出，等待sleep syscall时切换进程。其次，xv6 周期性的强制切换进程。通过内存allocator和页表为进程创建了一个 自己的虚地址空间。

实现复用有些挑战
第一是如何切换进程，尽管上下文的想法很简单，但是实现起来还是有些难以理解。
第二，强制切换对用户进程透明。
第三，多cpu并发切换进程，需要锁。
第四，用户进程无法回收自己的内核栈
第五，每个core都要记得自己正在执行的进程，这样syscall可以影响对应进程的状态
第六，sleep和wakeup允许进程放弃cpu进入睡眠，等待事件。允许其他进程唤醒第一个进程。


## 7.2 上下文切换代码
切换进程的步骤
1. trap进内核线程
2. 切换到scheduler线程
3. scheduler切换到新进程的内核线程
4. 内核线程返回用户区域

scheduler是每个cpu专用于调度的线程。如果scheduler在旧进程的内核栈中，当其他cpu唤醒旧进程的时候，就会有两个cpu在该进程内核栈上运行，很可能会造成错误。

swtch函数不关心线程，仅仅保存和恢复contexts。
切换进程时，内核线程调用swtch保存当前的寄存器组，然后恢复scheduler线程的上下文.swtch的工作就是保存旧contexts,加载新contexts

yield 调用sched 进入cpu的调度线程，进行调度。
swtch仅仅保存被调用者保存的寄存器，调用者保存寄存器由于c代码的调用规范已经被保存到栈上了。swtch不保存pc,仅仅保存ra就够了，ra是swtch被调用处。当swtch执行完成后，swtch返回的位置就是新进程上一次调用swtch的位置，所以进入了新进程


## 7.3 调度代码
进程想要放弃cpu必须获得自己进程的锁，然后释放其他的锁，更新自己的状态，然后调用sched进入调度进程.

xv6拿着上一个进程的锁进了swtch。调用swtch方必须拿到这个锁，并且把锁的控制权传递给切换后的代码

内核线程总是在sched里放弃cpu,切换到scheduler里

有一种情况，调度器调用swtch后没有返回sched。当新进程第一次被调度，他会开始于forkret，forkret的目的是释放自己的锁，然后调用usertrapret返回用户空间。


scheduler中遍历整个process table 寻找可以运行的进程，找到后就设置当前的进程为该进程，然后修改状态，切过去。

锁保护了进程的一致性。
比如当进程running的时候，时钟中断必须可以切走进程,这表示cpu必须持有进程的寄存器值用于保存到上下文中，当前进程也必须指向该进程。
比如一个进程runnable,则必须可以安全的切换到该进程上，这表示context中必须存着上下文，没有cpu在该进程的内核栈上执行，没有`c->proc`指向该进程，

为了保持一致性，xv6 总是在一个线程上 获取锁，在其他线程上释放锁。
比如yield中 获取锁然后再scheduler里释放
一旦yield开始修改进程状态，锁必须被持有，直到invariants被恢复。


## 7.4 mycpu myproc的代码
xv6有一个结构体叫`struct cpu`，保存着进程运行在的cpu信息，以及cpu的调度进程的信息，还有需要关闭中断所需的嵌套自选锁的数量。

确保cpu的tp寄存器始终保存着cpu的hartid是挺难的。mstart设置这些寄存器在cpu启动的时候。 usertrapret 保存了tp到trampoline。最终uservec 在进入内核时恢复tp.编译器保证不用tp进存器。

cpuid和mycpu的返回值是很脆弱的。当始终中断导致线程切换的时候，返回值就不可用了。为了避免这种问题，xv6 需要调用方关闭中断直到使用完毕返回值。


## 7.5 sleep and wakeup
xv6使用 sleep and wakeup这个机制去让进程等待某个事件，让其他进程唤醒睡眠进程，当事件来到的时候。

生产者消费者模型: pv操作

```c
producer(){
    lock();
    cond++;
    unlock();
}
consumer(){
    while(cond == 0);
    lock();
    cond--;
    unlock();
}
```

上述实现的开销很大，因为生产者的生产的慢，消费者耗费了大量时间在空转。

```c
producer(){
    lock();
    cond++;
    wakeup();
    unlock();
}
consumer(){
    while(cond == 0)
        sleep();
    lock();
    cond--;
    unlock();
}
```
这样可以让消费者不再忙等。但是还是有一个问题，当消费者判断完cond==0后，生产者生产了，然后wakeup发现没有进程睡眠，之后消费者再进入睡眠，就出现了lost wakeup的现象。
我们也不可以把锁移动到while之前，因为那样消费者会带着锁睡眠，导致生产者无法继续生产。
我们可以改变下sleep的接口
`sleep(cond, lock)`
sleep在进入睡眠时会释放锁，唤醒之后会去拿锁，上述代码会改为
```c
producer(){
    lock();
    cond++;
    wakeup();
    unlock();
}
consumer(){
    lock(lock);
    while(cond == 0)
        sleep(cond,lock);
    cond--;
    unlock(lock);
}
```
这样也不会出现死锁的情况


## 7.6 sleep and wakeup 实现

sleep 先要到了pcb的锁，然后释放了sleep传进来的锁，修改进程状态，调用sched被调度.在调度进程中释放了pcb的锁之后被调度到其他进程。
wakeup 唤醒了所有等待chan的进程，在pcb锁保护下将唤醒进程状态修改为runnable. 当scheduler重新调度到 被唤醒的进程时，他会在sleep的后半段开始执行，获取sleep传进来的锁。
**如果sleep传进来的锁就是pcb锁，则在sleep内不需要任何锁操作了，因为进sleep之前就拿到了锁，然后调度之后会释放pcb锁，在scheduler中上cpu前又会获得锁，返回到sleep后半段**

要pcb锁的操作不仅是为了修改pcb,还有一个情况就是防止wakeup和sleep不会错过，当wakeup执行时，其他进程就进不去sleep,反之亦然。

sleep被套在循环里的原因就是:wakeup唤醒了所有在chan上的进程，如果一个消费者就消费完了产品，则其他消费者必须继续sleep.

## 管道实现
更复杂的生产者消费者同步的例子就是管道。
pipe被表现为一个struct,内有一把锁和一个数据buffer.
当piperead和pipewrite 同时在两个cpu上运行时，pipewrite会获取pipe的锁，为了修改pipe的计数值和数据。piperead也会获取锁，但是获取不到，所以sleep了。


pipe使用了两个信号量nread和nwrite，可以让系统更加有效率(本质上是两个读写指针，写管道加写指针，使用nread唤醒读进程，读管道反之)


## 7.8 wait exit 和kill
wait中就是用了sleep
exit将进程设置为zombie,直到父进程发现并释放他后改为unused.
wait 使用了 进程的锁为了防止lost wakup (在exit中也会要父进程的锁)
xv6必需遵守相同的加锁顺序(父到子)去避免死锁
在wait查看进程表的时候 查看了np->parent字段，并未加锁，这看起来可能会导致死锁，但是这个字段仅仅可能被np的父进程修改，所以这样操作是安全的。

exit记录了退出状态，释放了一些资源，将所有子进程都给了init进程，唤醒了进程的父进程。这里有一点技巧，修改子进程的状态前获取了父进程的锁，然后是子进程的锁。
获取父进程锁的目的是为了让父进程回收在exit释放父进程锁后才能执行。
获取子进程锁的目的是为了让父进程确定子进程的exit执行完成后回收子进程.

kill做的事情很少，仅仅是设置killed = 1,然后设置进程为runnable.之后进程会在系统调用或者时钟中断的时候进入内核 被杀掉。

有些xv6的睡眠循环中不检查killed,因为有些系统调用的中间代码被视为原子的。virtio就是这样的:因为而i磁盘操作可能是一组读写，必须按顺序完成才是一个正确的状态。一个进程在等待磁盘io时 killed 改为1，的话 知道他完成了当前的系统调用和usertrap才会看到killed标志

## 7.9 真实世界
xv6的调度器实现了一个简单的调度规则，被称作round robin 轮询。真实的os实现了更复杂的规则，比如进程有优先级。
但是这样可能会有些意想不到的效果：多个进程共享一把锁，优先级低的进程要到锁后，可能有很多高优先级的进程等待锁。

最初的unix的sleep会关闭中断，因为unix运行在单cpu的计算机上。xv6是多处理器操作系统，添加了sleep中明显的锁。
freeBSD的msleep采用了同样的方法。plan9的sleep采用了回调函数，在sleep前获得锁时被调度。回调函数作为最后一分钟的睡眠条件检查，去避免lost wakeup.
linux kernel 的sleep使用了进程队列。每个队列有自己的锁。


扫描整个进程表去唤醒进程的效率很低。更好的做法是用一个数据结构代替chan,这个结构中有一个进程队列，存放着所有等待的进程。

信号量经常被用在同步上。

终止和回收进程是一件很复杂的事情。比如有些进程在内核里sleep,解开他的栈需要更细心的编程。大多数os在异常处理中使用显式的机制释放栈，比如longjmp.而且有很多其他的事件可以唤醒进程，即使进程等待的事件没有发生。比如unix进程sleep了，其他进程发送了signal给这个进程。这种情况下进程会从中断的系统调用中返回-1.

xv6对kill的支持并不完全令人满意。sleep 循环应该检查killed.一个相关的问题是：即使sleep循环检查了killed,还是存在一个race在sleep和kill之间。后者可能会设置killed时正好在检查killed之后，调用sleep之前，这种情况下，进程只有在自己等待的事情发生后才会注意到killed.：


# 7 filesystem
文件系统有七层
1. fd
2. pathname
3. directory
4. inode
5. logging
6. buffercache
7. disk

## buffercache
buffercache 有两个工作：
1. 同步对磁盘块的访问，只有一份块拷贝存在内存中，在一个时刻只有一个内核线程使用这个拷贝
2. cache 常用的块，以便使用

bread 获取一个buf 用于修改或者读取。
bwrite 则将buf写入disk ，内核线程必须释放buf 通过brelse

buffer使用sleeplock保证同一时刻一个buffer只有一个线程使用，bread返回了一个上锁的buffer,brelse释放了这个锁

buffercache有固定个数的buffer存放磁盘块。

## buffercache实现
buffercache是一个双向链表
初始化是初始化成一个环。

buffer有两个state变量，一个是valid,一个是disk,disk用来表示buffer的内容是否已经写入disk

### bread
bread 调用bget 获取buffer，如果block已经被cache了，就返回对应buffer指针，否则从disk中读到buffer里返回

每个disk sector只能有一个buffer.



### 块分配器
文件存放在磁盘块中，磁盘块是由分配器分配的，分配器维护一个空闲块的位图，每个为代表一块，0为空闲，mkfs设置了引导扇区，超级块，日志块,inode块和位图块。

块分配器用balloc寻找位图中为0的块，更新位图并返回该块。
bfree找到正确的位图块，清除对应位。

### inode
inode有两种意思
1. 指包含文件大小和数据块编号的磁盘上的数据结构
2. inode指内存上的inode,包含磁盘inode的copy以及内核所需要的额外信息

磁盘的inode被放在inode块的连续磁盘块内。
磁盘中的inode由`struct dinode`定义，定义了一些文件的元数据。

内核将活动的inode集合存放在`struct inode`中，iget和iput用于获得或释放inode指针，并修改refcnt.
inode是直写的，修改了inode之后要立刻用iupdate写回磁盘,

### inode实现
ialloc类似balloc,遍历inode找到空闲的，写入type,之后调用iget，iget设置inode的相关数据，并返回该inode。bget对bcache的访问是串行的，ialloc可以进程安全。

iget查找icache中对应的inode,找到就返回对其的引用，否则就记录下空inode的位置，用于分配新的inode.

iput减少refcnt,释放指向inode的c指针，如果refcnt为0,则清空inode,方便下次分配使用。如果iput进来的inode没有link,refcnt为1，则需要释放inode以及其数据区域，并将文件截断为0

### inode内容
inode中的数据部分分为直接索引和间接索引
bmap根据要读取的数据块号来进行查找块地址
itrunc 释放文件块，先设置inode的size,然后释放直接块和间接块。

bmap让readi和writei很容易实现。

### 目录层
目录实现很向文件，inode的type为`T_DIR`.
目录有一系列entry,entry包括了name和inode编号。
dirlookup在目录中查找dirent
dirlink 为目录添加一个新的dirent,只有名字和inum。

### 路径查找
namei计算path并返回inode

### fd层
fd层是保证抽象文件的一层。
fd指向的是一个`struct file`结构体，包含了inode或者管道的信息。
ftable是全局的文件打开表，有filealloc,filedup,fileclose fileread和filewrite的函数
filestat fileread filewrite对文件进行读写，调用下层的api比如pipe相关的api
inode锁 的读取和写入偏移量以原子形式更新，所以对同一个文件的写不会被覆盖，而是交错的写入。

### 系统调用
syslink为一个inode创建一个新的link，目录无法link所以如果inode是目录直接返回-1：
查找到new的父目录，然后在该目录的entry里添加一条new指向old 的inode即可

create 为新inode创建一个名称。


### logging
```c
begin_op()

end_op
```



### real world
实际操作系统的buffer cache比xv6复杂许多

