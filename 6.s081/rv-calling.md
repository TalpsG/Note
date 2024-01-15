# riscv调用convention
## c的数据类型和对齐
ilp32:int long pointer都是32位
rv32 isa都是ilp32的，而rv64则是lp64的


float:32
double:64
long double:128


char和uchar都是unsigned integer ，在存储的时候都是零扩展存储到寄存器中的

short和ushort存在寄存器中的时候会有符号之别


## rvg 调用规范
当可以用寄存器传参的时候，会使用a0-a7,浮点数用fa0-fa7.

可变参数的函数参数即使是浮点数也会用整数寄存器传递。

在寄存器传参的时候也要考虑到对齐的原则
`void foo(int a,long long b)`,这个函数的a参数通过a0进行传递，b参数通过a2,a3共同传递，a1由于对齐的缘故不参与传参。

参数大于两倍指针长度则会传递指针

struct参数通过栈传递.

除了struct内有两个float或是long double的时候，其他时候都用a0a1传递参数
更大的返回值则通过内存传递: caller开辟内存空间并传递指针给callee,作为callee的第一个隐式参数

标准的riscv calling convention中 栈是向下生长的，且永远保持16字节的对齐

t0-t6,ft0-ft11是caller saved寄存器

## soft float calling convention
soft-float 是用在实现了rv32或rv64 但缺少浮点硬件的微架构上的。
避开了所有FDQ标准扩展指令，也避免了f寄存器


这个调用规范中整数参数的传递和返回与之前一样。
浮点数参数的传递和返回通过整数寄存器实现

