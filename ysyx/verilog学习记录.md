# verilog
## basics
### wire
创造一个module,他有一个输入和一个输出，这个module表现的和一根线一样。

不像物理的线，verilog中的线是有方向性的，代表信息总是从一段流向另一端，在verilog中使用`assign a = b` 来代表b的值流向a

**assign语句赋值的应定义成wire,而always内被赋值的应该被定义为reg**

module内的ports也有方向，input是向mod内输入，而output是mod向外输出
```verilog
module top(input in,output out);
assign out = in ;
endmodule
```

### wire4
```verilog
module top(
    input a,b,c;
    output w,x,y,z
);
assign w = a; 
assign x = b; 
assign y = b; 
assign z = c; 
```

### wire5 
取反
```verilog
module top_module( input in, output out );
    assign out = ~in;
endmodule
```

### wire 关键字
`wire`的目的是为了引出中间的结果，比如我想用a&b的值，我可以用一个wire来等于a&b而不用一直写a&b

示例
```verilog
module top_module (
    input in,              // Declare an input wire named "in"
    output out             // Declare an output wire named "out"
);

    wire not_in;           // Declare a wire named "not_in"

    assign out = ~not_in;  // Assign a value to out (create a NOT gate).
    assign not_in = ~in;   // Assign a value to not_in (create another NOT gate).

endmodule   // End of module "top_module"
```

### 7458 chip
```verilog
module top(
    input p1a,p1b,p1c,p1d,p1e,p1f,
    output p1y,
    input p2a,p2b,p2c,p2d,
    output p2y
);
    wire p2ab,p2cd,p1abc,p1def;
    assign p2ab = p2a&p2b;
    assign p2cd = p2c&p2d;
    assign p1abc = p1a&p1b&p1c;
    assign p1def = p1e&p1d&p1f;

    assign p1y = p1abc|p1def;
    assign p2y = p2ab|p2cd;

endmodule

```
## vectors
### vector 0
`wire [7:0] w`的意思是w是一个含有8个数据的向量，等同于八根分开的线，类似于数组

如果两个vector的宽度相同可以直接进行assign赋值。
如果需要抽出vector内的某个元素可以使用下标
`assign a = vec[0]`

### vector的声明

```verilog
wire [7:0] w;
reg [4:1] r;
output reg [0:0] y; //一个bit的寄存器，但也是一个向量，也是一个输出端口
input wire[3:-2] a; //负数是允许的
output [3:0] b; //默认为wire
```

vector不可以赋值给其他的元素，否则容易出现bug

vector的赋值类似python的切片
`vec1[10:0] = vec2[10:0]`


`output [10:0] w [255:0]`代表255个11bit的输出端口

### 位操作符和逻辑符号
位操作：| & 等
逻辑操作： || && 

### 拼接
`{out[7:0],out[12:5]}`
可以将两个元素拼在一起，可以使用在assign语句中
'{num{vec}}'
num代表个数,这个的结果是一个vector

#### 数据如何写

`6'b111000` 代表着这个数据是6bit，二进制数，111000,同时我们可以把b换为h(16进制)等，这样的话数据的大小也会跟着改变。

## Module
实例化module的时候可以通过位置和名字来给module的port与本module内的signal进行连接。
```verilog
module mod_a(
    input in1,in2,
    output out
);
endmodule
mod_a instance1(a,b,c); //按位置 abc为in1 in2 和out
mod_a instance2(.out(a),.in1(b),.in2(c)); //按名字分，让out为a以此类推
```


## 过程

`<=`在块结束也就是`end`后才会完成赋值
`=`会立马赋值
`assign`中使用`=`
`always`中使用`<=`

**在always块内，`<=`会被并行执行，而`=`会被顺序执行**
### begin和fork
begin代码快顺序执行
fork代码块并行执行
### always语句
`always @(*) x = y`等价于`assign x=y`


### always @(posedge )
`always @(posedge clk) x = y`意为在clk上升边沿时执行`x=y`

### if
```verilog
if (condition) begin
    //语句
end
else begin
    //语句
end
```
与c语言不同的是，大括号被`begin`和`end`代替了

如果现在的情况是我们并未写到的case,那么verilog不会改变输出，但是组合逻辑并不会保存状态，这样的情况就会出现警告，可能会导致出现bug,所以要尽可能的去覆盖所有的case,并且编写一个default case去对output进行赋值

### always case  很像switch
```verilog
always@(*) begin
    case(var)
        val1:code block
        val2:code block
        default: default code
    endcase
end
```
### casez
casez对值为z的位不进行比较

## verilog 特性
### 三目运算符
和c一样

### 单操作数的运算符
`&(|,^)a[3:0]` 意为a的每一位之间都做与或异或运算

## 重要的一些原则
1. 组合逻辑中使用`=`
2. 时序逻辑中使用`<=`
3. 在同一个always内只能有一种赋值方式
4. 一个信号只能在一个always内或一个assign赋值
5. 原则上来说，一个always内只处理一个或一类信号，不同信号可以用不同的always块内处理(充分利用并行执行代码块的原则)
6. always内只对reg信号处理，部队wire赋值，也不实例化模块

## 一些电路
### 带同步复位的D触发器
```verilog
module D_wit_reset(
    input clk,
    input [3:0] d,
    input reset,
    output reg [3:0] q
);
    always@(posedge clk) begin
        if(reset)
            q<=4'b0;
        else
            q<=d;
    end
endmodule

```
### 异步复位的D触发器
```verilog
module D_wit_reset(
    input clk,
    input [3:0] d,
    input reset,
    output reg [3:0] q
);
    always@(posedge clk ,posedge reset) begin
        if(reset)
            q<=4'b0;
        else
            q<=d;
    end
endmodule

```

### 有限状态机
parameter类似常量
```verilog
module divideby3FSM(
    input clk,
    input reset,
    output q
);
parameter S0 = 2'b00;
parameter S1 = 2'b01;
parameter S2 = 2'b10;

reg [1:0] state;
reg [1:0] nextstate;

always@(posedge clk,posedge reset) begin
    if(reset) state<=S0;
    else state <= nextstate;
end

always@(*) begin
    case(state)
        S0: nextstate=S1;
        S1: nextstate=S2;
        S2: nextstate=S0;
        default: nextstate=S0;
    endcase
end

assign q = (state==S0);
endmodule
```

### 参数传递
定义模块的时候可以通过`#`来定义模块内的parameter,在实例化模块的时候也可以通过`#`来修改paramete
```verilog
input   s,
output [width-1:0] y
);
assign y = s?d1:d0;
endmodule
```


## 从电路角度入门Verilog
可综合语句
先想电路，在写代码

互连，并发，时间

### 时延
`wire #10 a=1'b0`
代表着a的赋值延时10个单位，也可以在`assign`语句中使用延时，但是不推荐

惯性时延：是指在给变量赋值延时期间依赖的变量发生了变化，就需要使用新的变量值来计算。

### initial语句块
initial只执行一次，多个initial并行执行



