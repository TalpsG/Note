# backtrace
打印栈上的返回地址即可

编译器在函数开始时会把栈底位置存在s0处
s0-8存放返回地址
s0-16存放上一个stackframe的栈底(实际上是高地址，slides上写的bottom和top,和这里的顶和底是反着来的)


# alarm
在usertrap里执行切页表，执行对应函数，发现换了页表后无法访问下一条指令了!!

## test1
很简单直接在usertrap里把trapframe的epc置为handler即可
## test2
按照test1的方式进程就会一直执行handler,回不去用户程序了

为了返回用户程序，需要sigreturn调用
由于一直执行handler,原用户程序的上下文被handler修改了，所以要在第一次调用handler之前把上下文保存下来
可以创建一个alarmframe,直接赋值trapframe的内容即可，在sigreturn处还原trapframe.


