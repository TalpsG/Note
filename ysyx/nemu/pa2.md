# pa2
## 不停运算的机器

取值：pc
译码：机器码，类似于查表
执行：
更新pc


```c
typedef union {
  struct { uint8_t rs : 2, rt : 2, op : 4; } rtype;
  struct { uint8_t addr : 4      , op : 4; } mtype;
  uint8_t inst;
} inst_t;
```

注意`op`是高4位。

## RTFM
1. 指令的具体行为
2. 操作码的编码表格


### 取指令
内存访问罢了

### 译码
`INSPAT`
`INSPAT_MATCH`

#### 小段
`0x1234`在存储时实际上是这样的
`34,12`低位字节存储在低地址部分，而高位存放在高地址部分

### 解耦

把取指令，译码和执行三个阶段分开来做。

### 一条指令的执行过程
`execute`函数创建一个decode 结构体，结构体内存放了指令相关的信息，包括snpc和dnpc以及指令本身等等,之后调用`exec_once`
`exec_once`执行一条指令,将pc和snpc设置后执行`isa_exec_once`，执行isa相关的部分
`isa_exec_once`调用了`inst_fetch`取指令，然后对指令进行译码以及执行`decode_exec`
取指令就不谈了，只是访存罢了
在译码函数中发现两个宏定义了,并且使用其进行指令的匹配，确定现在取出的指令是属于哪条指令，要执行什么行为。(nemu默认实现了五条指令)

`INSPAT` 调用了`INSPAT_INS`用来取出指令进行进行匹配指令，之后调用了`INSPAT_MATCH`用于解析指令，将操作数提取出来并且执行对应的操作,执行完毕后返回之后就一步一步返回了，与指令执行的关系就不大了
值得一提的是用户程序的退出是因为ebreak代码内的行为改变了nemu的state

