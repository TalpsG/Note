# verilator
verilator 可以把verlog等硬件描述语言HDL设计转化成cpp或systemC的模型。
它是一个编译器而非传统的模拟器

## overview
模拟需要用户写一个cpp的包装文件。

## 安装
参照verilator官网的文档

## example
```cpp
#include "Vour.h"
#include "verilated.h"
int main(int argc, char** argv) {
  VerilatedContext* contextp = new VerilatedContext; 
  //传送上下文
  contextp->commandArgs(argc, argv);
  Vour* top = new Vour{contextp};
  //实例化module
  while (!contextp->gotFinish()) { top->eval(); }
  delete top;
  delete contextp;
  return 0;
}
```

`verilator -cc --exe --build -j 0 -Wall sim_main.cpp our.v`

1. --cc 获得c++的是输出
2. --exe 由于写了sim_main.cpp这个包装的文件，所以可以编译出来一个可执行文件，而不是库
3. --build verilator自己构建，也可以用户自己写构建的规则
4. -j 0 make时尽可能多的使用cpu的线程技术
5. -Wall 输出全部的warnings
6. our.v 是systemverilog 设计文件

## 双控开关
```verilog
module top{
    input a,
    input b,
    output f
};
assign f = a ^ b;
endmodule
```
```cpp
#include "Vtop.h"

int main(int argc, char** argv) {
    // See a similar example walkthrough in the verilator manpage.

    // This is intended to be a minimal example.  Before copying this to start a
    // real project, it is better to start with a more complete example,
    // e.g. examples/c_tracing.

    // Construct a VerilatedContext to hold simulation time, etc.
    // 构建一个verilated上下文去hold住模拟的时间
    VerilatedContext* contextp = new VerilatedContext;

    // Pass arguments so Verilated code can see them, e.g. $value$plusargs
    // This needs to be called before you create any model
    // 把命令行的参数传递进去
    contextp->commandArgs(argc, argv);

    // Construct the Verilated model, from Vtop.h generated from Verilating "top.v"
    //构建一个module 
    Vtop* top = new Vtop{contextp};

    // Simulate until $finish
    while (!contextp->gotFinish()) {

        // Evaluate model
        // 评估模型
        top->eval();
    }

    // Final model cleanup
    // 最后的模型清理
    top->final();

    // Destroy model
    // 销毁
    delete top;

    // Return good completion status
    return 0;
}
```

## verilator 的选项
`-f` 指定parse的文件
`--coverage` 覆盖所有，应该是调试相关的选项

## 生成vcd文件的example
```cpp
int main(int argc,char *argv[]){
	VerilatedContext *contextp = new VerilatedContext;
	Verilated::traceEverOn(true);
	VerilatedVcdC *trace = new VerilatedVcdC;
	contextp->commandArgs(argc,argv);
	Vtop* top = new Vtop{contextp};
	top->trace(trace,5);
	trace->open("waveform.vcd");
    //读写这个文件
	while(simtime<MAXSIMTIME){
		int a = rand()&1;
		int b = rand()&1;
		top->a = a;
		top->b = b;
		top->eval();
		trace->dump(simtime);
        //记录当前时间点处的trace
		assert((a^b)==top->f);
		simtime++;
	}
	top->final();
	trace->close();
	delete top;
	delete contextp;
	return 0;
}
```

## 双控开关的波形图
verilate的指令是
`verilator --cc --exe -x-assign fast -Wall --trace --assert --coverage -f inputfile`  
`--trace` 是开启波形图生成

构建可执行文件的指令是
`make -j -C obj_dir -f ../Makefile_ob`


## verilator学习

### 介绍
verilator是cyclebased的仿真器
cycle-based 不去模拟具体电路的时序，而是计算电路在每个周期的稳态响应，用户看不到电路的错误行为，只能仿真同步设计
event-based 当有一个输入事件的时候才进行eval

verilator不支持大多数non-synthesizable代码，支持$display(),$finish(),$fatal()和一些版本的$assert()

### verilog to cpp
--cc选项就是可以将verilog的代码转换成cpp代码

转换后会生成Vtop.h和Vtop__unit.h
Vtop.h 包含了Vtop类的定义
另外一个文件包含了操作符号的定义

