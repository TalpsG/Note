# 9.1
## 中断硬件部分
中断的特征
1. 异步
2. 并发
3. 程序化

cpu通过Platform Level Interrupt Control PLIC来处理外部中断。PLIC接受中断并将其传递到对应的cpu上，对应的cpu会对中断进行claim.

## 设备驱动概述
驱动大部分分为两个部分:
1. 顶部一般是read write等接口
2. 底部一般是handler，cpu接受到中断会调用这个handler,他不关心上下文，仅仅处理中断

驱动中通常会有buffer,top部分会从buffer中读取数据，bottom也会在队列中读写数据。
对中断处理来说存在一些限制，因为他没有运行在进程的上下文中，所以无法对虚拟地址进行转换。top部分常与用户程序交互。

很多os中驱动代码比oskernel还要大，因为设备多且杂。

## console如何显示出内容
一些寄存器
SIE: supervisor interrupt enable 寄存器，有一个位专门针对uart的外部设备中断
SSTATUS: supervisor status 寄存器有一个 位可以打开和关闭全部中断
SIP: supervisor interrupt pending 寄存器 用于存放中断类型
SCAUSE: 当前状态的原因是中断
STVEC: 指向中断处理的代码

start.c 中start函数设置了mstatus，mepc为了mret.然后将所有的异常和中断都设置在s模式
之后初始化timer。 然后mret跳到main函数处。main先对consoleinit(uartinit)，之后printfinit后就可以用printf输出内容了

由于plic还没有初始化，所以cpu并不能接收到中断请求。在plicinit中设置中断的优先级，然后进入plicinithart,为各个cpu设置感兴趣的中断。此时plic就可以转发中断到cpu,但是我们还没有设置sstatus,中断使能还没有开启。在scheduler里开启了中断后cpu就可以接收中断了。

### 驱动中top部分
init是第一运行的进程，他open了console，然后复制console的fd到stderr和stdout。
printf最终会调用write，在内核中的调用链是:
sys_write->filewrite->consolewrite->uartputc


### 输入的回显
$和ls还不一样，因为$是cpu发送出来的，而ls是cpu接收到并发送出来的。对于$发送完成后产生一个中断，然后qemu模拟另外的部件使他显示出来。而ls则是cpu借由uart接收到的数据，再借助uart发送到qemu模拟的显示部件显示出来

### bottom 部分
我们向console输出字符会发生什么？
当我们敲击键盘，键盘借助uart发送给cpu一个中断，此时会发生:
1. 设置SIE关闭中断，防止cpu被打扰，处理完成后会回复SIE
2. 设置SEPC保存断点，保护现场并跳转到对应的vec程序中
在处理程序中我们最终会调用devintr,在该函数内会使用plic_claim函数接收中断，并判断中断的类型。调用对应的处理函数.由于我们输入了ls,然后buffer非空，会读取到数据，然后会调用consoleintr对字符进行输出

### 中断的并发
1. buffer的使用，driver和用户程序是生产者消费者模型
2. 内核的部分代码有时不能被中断
3. 对于同一个uartbuffer有可能多个进程访问，需要加锁

