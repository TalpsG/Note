# chisel 学习
## 1 信号类型和常量
### 三种基本信号：UInt,SInt和Bits，其中Bits没啥用

```scala
Bits(8.W)
UInt(8.W)
SInt(8.W)
```


### 常量的定义很简单
`0.U`,`-3.S`,`3.U(4.W)`
数字后面跟一个数字的类型即可，也可以在后面继续定义常量的宽度。
宽度未定义则会自动推断最小的宽度。

或者可以用其他进制来定义常量
`"hff".U`,`"o377".U`都是255

也可以用ascii来表示常量
`'A'.U`的值为`A`的ascii值
**注意要用单引号**



## 2 组合逻辑和运算符
### 运算符
#### 位运算符
`&|~^`

#### 移位,
移位运算符对有符号数的移位是算数移位 `<< >>`
注意：
加法的位宽为最宽的操作数的位宽
乘法为位宽之和
除法为被除数的宽度


##### 等号与不等号
等于号和不等是这样的`===  =/=`,这样的目的是为了让`scala`中原本有的符号仍然可用


#### 规约运算符号
```scala
val a = x.andR
val b = x.orR
val c = x.xorR
```


#### 位操作符
```scala
val xLSB = x(0) #取最低位
val  a = x(15,12)  #取12 到15 四位
val c = Fill(3,"hA".U) # c为0xaaa
```

Fill是一个操作符，**只能复制UInt,返回的也是UInt**,第一个参数是复制次数，第二个是复制的向量，类似于verilog中的`3{3'b11}`


#### 拼接字符
```scala
val float = Cat(a,b,c)
val float2 = a ## b ## c
```

`Cat`和`##`的用法是有区别的，不多说，操作数尽量都为UInt
`Cat`生成的verilog代码是常量，而`##`得到的代码是`{}`这样的


## 3 寄存器和计数器

### 寄存器
`val reg = RegInit(0.U(8.W))`

**注意信号赋值时需要使用`:=`**

`RegInit`可以定义一个有初值的寄存器
`RegNext`既定义了初值，也给给定了输入
`val r1 = RegNext(d,0.U)` r1的初值就为d


## 4 Bundle和Vec
bundle可以将不同类型的信号组合成一组信号
vec类似数组，表示相同类型的信号集合
### Bundle
```scala
class Channel() extends Bundle{
    val data = UInt(32.W)
    val valid = Bool()
}
val ch = Wire(new Channel())
ch.data :=123.U
ch.valid := true.B
```

### Vec
```scala
val v = Wire(Vec(3,UInt(4.W))
v(1) = 1.U
val idx = 1.U(2.W)
val a = v(idx)
```

上述代码将一组信号封装到了Wire中，可以通过索引选择信号通过。

`val regfile = Reg(Vec(32,UInt(32.W)))`
这行代码则定义了32个32位的寄存器，可通过`regfile(idx)`来访问对应下标的寄存器值

### Vec和Bundle的组合


我们可以创建bundle的vec,比如`val vecBundle = Wire(Vec(3,new Channel()))`

也可以在Bundle内创建Vec
```scala
class bundleVec extends Bundle{
    val vec = Vec(4,UInt(8.W))
}
```

我也可以创建一个Bundle类型的寄存器，代码如下
```scala
val bundle = Wire(new Bundle())
val channelreg = RegInit(bundle)
```


### 部分赋值
chisel中是不可以部分赋值的，但是我们可以借用Bundle实现这一目的

```scala
val assignWord = Wire(UInt(16.W))
class Split extends Bundle{
    val high = UInt(8.W)
    val low = UInt(8.W)
}
val split = Wire(new Split())
split.low := lowByte
split.hight := highByte

assignWord := split.asUInt
```


## 5 Wire Reg 和IO 以及如何理解Chisel生成硬件
**数据类型类似`UInt`实际上并不表示硬件，必须封装成`Wire`,`Reg`或`IO`才会生成电路。
### Wire Reg 和 IO
`Wire`代表组合逻辑
`Reg` 为寄存器
`IO`则为模块的接口


在`scala`中`var`和`val`都可以声明变量，`val`是不可变的，和电路一样，所以我们只是用`val`来描述电路

`:=`是给已经存在的硬件赋值使用的，创建新的硬件使用`=`就可以

组合逻辑也可以进行条件赋值，但是需要给每个分支都赋值，否则会出现`latch`锁存器，`chisel`会报错，所以可以给`Wire`一个初始值

`val num = WireDefault(10.U(4.W))`
意思为`num`默认就连接在一个信号上



## 6 chisel生成verilog & chisel开发流程

### 生成verilog

```scala
object Hello extends App{
 emitVerilog(new Hello(),Array("--target-dir","generated")) # 生成代码保存在generated文件夹下
 println(getVerilogString(new Hello()))  # 打印Verilog出来
}
```

## 7 scala test

