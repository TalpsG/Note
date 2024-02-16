# virtual machines
## why virtual machines
比如公司对外出售云服务功能，可以将一台物理机分成几台虚拟机出售。

虚拟机的初衷是完全模拟物理机，让系统感知不到自己在虚拟机当中，但是出于现实的性能考量，guest系统是会和vmm进行交互的，也可以感知到自己是否运行在vmm之上。

## trap and emulate -> trap
如何构建自己的vmm？

一种可行的方法是编写一个模拟物理机运行的软件，读取指令，模拟指令执行，用变量模拟寄存器与内存空间。 这种方法很简单也很直观，但是很慢，因为其运行效率很低。

另外一种方法是在真实的cpu上执行guest指令，当然模拟器架构要和本地的架构相同才行。不过有些指令不可以直接执行，比如特权级相关的指令(csrrw等)。当执行这些指令的时候，会触发trap,走到vmm程序中，之后就可以在vmm中进行处理了。

## -> emulate
vmm 为每一个guest维护一套虚拟的状态信息，包括了一些特权级相关的寄存器。这样当特权级指令trap后就可以通过vmm进行修改对应的信息。

guestos的系统调用流程
1. 用户程序执行系统调用
2. ecall到guestos的内核中
3. guestos读取scause寄存器触发trap
4. vmm处理trap，修改对应os的scause寄存器为8并返回
5. 回到guestos内核然后就和正常的syscall一样了

vmm应当记录guestos当前运行的模式，user or supervisor。这样当用户程序要执行特权级指令的时候才会被vmm处理。

vmm可以通过ecall和sret等指令进行特权级切换,vmm可以通过检测指令来维护guestos的运行模式

vmm还需要维护hartid,也就是cpu核的编号,这个通过特权指令也无法直接获取，vmm需要维护跟踪这个信息。

当guest执行到sret时，由于vmm还处于usermode所以会触发trap然后进入vmm的代码，vmm修改对应寄存器，切换页表之后返回到guest

当guest运行ecall时会进入trap,进入vmm中，然后vmm修改对应的sepc，将虚拟的scause设置为系统调用，修改vmm维护的guest特权级，真实的sepc设置为虚拟的stvec(这样在sret的时候，返回的sepc位置就是系统内核中的traphandler).之后调用vmm的sret,跳转到guestkernel的traphandler.

## pagetable
trap and emulate还需要两个重要部分： 页表和外设
页表包含两个部分
第一个是satp寄存器，我们不能让vmm简单的修改真实的satp,那样guest就可以访问全部的地址空间了，vmm期望的是guest只能访问vmm分配的空间。
因此，vmm需要软件实现一个页表用来模拟guest的页表

所以guest的地址转换流程是这样的：
guest va->guest pa(handled by software in vmm pagetable)-> host pa (in real pagetable)


当guest写入satp时，通过traphandler中vmm会创建一个shadow pagetable 这个页表的地址会被写入satp中。
shadow pagetable的构建过程如下
1. 从guest的页表中查看gpa
2. 使用vmm的页表查看hpa
3. 最终将gva和hpa的映射写入shadowpagetable


这样的gva对应的hpa是在vmm可控范围内的。

缺页的流程
1. 缺页后进入vmm的traphandler，smode
2. traphandler中修改对应的stvec,然后修改guest的一些寄存器最后sret到guest的处理代码中，返回到umode
3. guest进行缺页处理,最后回到guest的用户空间。



## devices
三种策略模拟外设
1. mmio的方式，某些外设对应着一块虚拟地址，vmm将这些va对应的pte设置为无效，这样访问这些地址时会进入trap,此时vmm去查看对应的device中的数据。由于每次都要trap所以效率低
2. 虚拟设备,在vmm构建特殊的设备接口，比如一个写入队列和读缓存，批量写入设备等方式。
3. 直通方式，比如网卡，现代网卡具备硬件支持多个设备的能力。

## 硬件对虚拟机的支持

VT-x是英特尔提供的x86平台的虚拟化支持
trap n emulate中vmm通过trap修改软件中的控制信息。而有了VT-x之后guest运行特权指令后可以直接修改硬件中的虚拟寄存器，避免了trap的开销。

vmm会使用真实的控制寄存器，而vmm可以通知硬件切换到guest模式，硬件还有一套为guest模式使用的虚拟控制寄存器，guest模式下可以直接读写控制寄存器。

硬件会对guest进行一些约束保证其不会逃逸。

non-root mode 对应的guest,rootmode对应host.

在guestos中可以执行特权指令，硬件允许直接读写stvec寄存器的non-root拷贝。

当vmm创建新的虚拟机时，vmm需要配置硬件，vmm内存中通过vmcs(virtual machine control structure)来与VT-x进行交互。vmm新建虚拟机时创建一个新的vmcs,填写对应初始值。
intel增加了一些新的指令来实现这样的交互
- VMLAUNCH 创建新的虚拟机，传入vmcs的地址
- VMRESUME 有时guestkernel会通过trap进入vmm,vmm需要该指令恢复guestkernel运行
- VMCALL 在non-root下运行，退出non-root 通过trap进入vmm 

通过硬件支持，guest可以直接执行特权指令，但是还是有一些情况下需要进入vmm，比如定时器中断，设备中断等。

VT-x 还增加了页表的支持，在guest中仍然需要页表。guestkernel需要自己的页表，需要能够读写cr3。VT-x可以使guest加载值到cr3中，进而设置pagetable。硬件也会执行guest的指令,因此guestkernel不需要trap到vmm来加载pagetable了。

不过不可以让guest随意修改pagetable,VT-x有个ept,ept是vmm启动guest时准备的。

gva->gpa(real pagetable)->hpa(ept)


## Dune:安全的用户级访问特权CPU特性
论文使用硬件对虚拟机的支持做了一些其他的事情。
dune是一个可加载的模块，处于linuxkernel内部。
dune可以支持进程，也就是linux进程新增了一个dune模式，这个模式下进程被VT-x隔离开了，有完整的控制寄存器，可以执行特权指令。

由于dune管理的进程可以自行设置cr3寄存器，dune也会控制进程的ept,可以使进程不会从分配给他的内存中逃逸出来。

dune进程也可以拥有supervisor和user两个模式，进程可以在user模式运行未被信任的代码，比如浏览器插件等。

在dune中可以通过获取pte的某些位来加速某种操作。














