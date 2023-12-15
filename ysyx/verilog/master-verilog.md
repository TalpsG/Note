# verilog
## verilog 和c的区别
verilog是并发的
verilog的本质是电路

## 描述方法
1. 数据流： assign
2. 行为模型：c语言
3. 结构模型： 模块化

### 变量类型
线网和寄存器类型
always和initial内赋值的变量一定是寄存器类型
assign一定是线网类型

#### 线网类型
wire ，tri用于多驱动源建模
wor,trior 线或
wand,triand 线与
trireg 连线具有总线保持功能
tri1,tri0,当无驱动时连线状态为1和0
supply1和supply0 表示电源和地
#### 寄存器类型
reg
integer
time


### 变量的物理意义
线网对应连线
而reg并不一定代表寄存器
reg在语义的钓具来说是会在仿真中保持的变量
reg就像是c的变量，而wire则像是其他变量的引用

### reg和wire
由于wire的值一直都是根据变量决定的，所以需要assign来赋值，保证其一直都有值
而reg可以保存自己的值，所以在敏感信号发生时再赋值就可以

### verilog中的并发和顺序
```verilog
fork
join
```
是并行的语句，而begin和end是

