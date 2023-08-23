# Makefile
## 2
### 2.1 makefile 的规则

makefile的目的就是为了自动的编译与链接程序

```makefile
target: prerequisites
	command
```

如果 需要的文件中有一个比目标的文件更加新，则command会被执行。

### make 是如何工作的

make会在当前目录中寻找makefile文件，然后会递归的获取target文件，当prerequisites也没有或是比target新的时候，就会从新获取prerequisites。

### 变量

假设一个场景，编译target需要的pre很多，则pre处要写一串.o文件，而command中也要写一串，写得多也容易出错，这时我们就可以使用变量

```makefile
OBJECTS = a.o b.o c.o d.o e.o f.o....

main:$(OBJECTS)
	cc -o main $(OBJECTS)
```

这时OBJECTS会展开为.o文件

### make 自动推导
当target 为.o文件时，会把同名的.c文件自动加入pre中。
所以我们可以只写头文件，其他的文件通过推导加入pre，但是这样的makefile会显得很凌乱。

### clean

```makefile
.PHONY : clean
clean :
	-rm target *.o
```
clean不要放在makefile的开头，否则会成为默认的目标，**rm前的横杠**是代表出现问题不需要处理，继续执行

### 引用其他的makefile
`include foo.make`

## 3
### 通配符
```makefile
$@ # 目标文件
$< # 第一个依赖文件
$^ # 所有依赖文件
$? # 比目标文件更新的依赖文件列表
```
### 伪目标
像clean实际就是伪目标，因为clean并不生成文件，而只是执行命令。
伪目标也可以成为pre,比如`clean:objclean asmclean`
### 静态模式
```makefile
targets : target-parttern : pre-parttern
	commands
```

静态模式可以从targets中按照targets-parttern去选择符合模式的target文件，并添加上符合pre模式的依赖文件，此后执行commands.

### 自动生成依赖
`gcc -MM a.c`会生成 a.o所需要的依赖。
GNU建议给每一个c文件都生成一个d文件，d文件中存放c文件的依赖关系

```makefile
%.d:%.c
	@set -e:rm -f $@;\
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$;\
	sed '..' < $@.$$$$ > $@;\
	rm -f $@.$$$$
```
`$<`是指每一个目标文件对应的依赖文件.
第一行是删除目标文件.
第二行是指为每一个c文件生成d文件，其中d文件有一个随即编号.
第三行进行替换并写入最终的`$@`目标文件中.
第四行是指删除随机编号的文件.

`include $(sources:.c=.d)`
该行的意思是指将sources展开后的`.c`替换为`.d`


## 4
### 显示指令
`@echo blabla`
在执行makefile的时候会先打印要执行的指令，再打印执行结果，指令前加上@可以使make执行的时候不显示指令本身

`make -n(--just-print)`会指打印命令而并不执行，可以利用该指令来了解makefile是以什么顺序执行的

`make -s(--quiet/--silent)`会全面禁止指令的显示

### 命令执行
```makefile
exec:
	cd /home/talps
	pwd
```
```makefile
exec:
	cd /home/talps;\
	pwd
```
当第一个makefile执行时，pwd打印的makefile的目录，第一行的cd对第二行是独立的
第二个makefile则与预期相同。
当需要让第二条指令依赖第一条指令的时候需要在指令后加一个分号。

### 命令出错
make执行时如果指令出错会立刻结束，但出错有时并不代表真的有错。
比如当make clean时可能之前clean过了，导致rm指令执行失败。但是其实这并不是错误，我们可以在rm前加一个减号`-`,或者`make -i`,使make忽略错误,这样即使这条指令执行失败也会继续按照makefile执行。
`-rm *.o`

这里提一下`make -k`是指当某个规则执行出错后终止该规则，但是继续其他规则

### 嵌套执行makefile
```makefile
subsys:
	cd subdir && $(MAKE)
#或是	$(MAKE) -C subdir
```
上述代码可以执行subdir中的makefile,将其定义为一个变量是为了方便传递参数

父make调用子make可以把他的变量也传递到下级make中，但是并不会覆盖掉下级make中的变量，除非指定了 `-e`选项
想要传递某个变量到下级make中可以：
`export var = value`
如果不想传递到下级make中，可以这样声明
`unexport var`

如果想传递所有的变量只需要一个`export`就可以了

**有两个变量不管export都会传递到下层，分别是`SHELL`和`MAKEFLAGS`，还有一些make的参数并不想下传递，如果你不想参数可以这样：**
`$(MAKE) MAKEFLAG=`

`make -w(--print-directory)`会打印当前的目录信息

### 定义命令包
```makefile
define run-yacc
yacc $(firstword $^)
mv y.tab.c $@
endef
```
该段代码定义了一个run-yacc命令包
`$^`是指依赖文件，`$@`是目标文件

## 5 变量
### 变量基础
变量类似于c的macro,会自动展开，只需要`$(var)`即可
### 变量中的变量
```makefile
foo = $(bar)
bar = $(ugh)
ugh = Hub?
```
变量不分顺序，可以在后面定义前面需要的变量
同时也可以强制让变量按顺序定义，使用`:=`
如果使用了`:=`定义变量使用到了之后的变量，那么该变量展开处为空
`foo?=bar`的意思是如果foo没定义过，那么他为bar,否则什么都不做

### 5.3 变量高级用法
```makefile
foo :=a.o b.o c.o
bar :=$(foo:.o=.c)
```
这个例子中的bar为foo中后缀替换为c,也可以有以下的写法
```makefile
foo :=a.o b.o c.o
bar :=$(foo:%.o=%.c)
```


```makefile
x = var1
var2 :=hello
y = $(subst 1,2,$(x))
z = y
a :=$($($(z)))
```
a = $($(y)) = $(var2) = hello
`subst`是一个函数 将x变量中的1换为2

### override
当如果有变量是通过make 后的选项设置的，那么makefile中这个变量的值会被覆盖，如果想在makefile中不被覆盖，需要使用
`override var1; = value1`或
`override var1; := value1`

### 多行变量
```makefile
define twolines
echo foo
echo bar
endef
```
上述代码定义了一个名为twolines的变量，变量内容为下面两行
**注意如果想要定义命令，命令前要有tab**

### 环境变量
环境变量会自动载入makefile内
### 目标变量

作用范围仅在该规则内部及其连带规则中
```makefile
target: var = value
```
### 模式变量
目标变量可以用于模式
`%.o ;: var = value`

## 6
### 条件判断
```makefile
ifeq ($(CC),gcc)
    $(CC) -o foo foo.c $(GCCLIB)
else
    $(CC) -o foo foo.c $(OTHERLIB)
endif
```
ifeq关键字有其他
`ifeq 'arg1' 'arg2'`
`ifeq "arg1" "arg2"`

此外还有ifneq,在条件值为假时if进入true分支

`ifdef foo`在foo的值为非空时，表达式为真

**注意make是在读取make文件的时候计算条件表达式的值，所以不要把自动化变量放入条件表达式中**

## 7函数
### 函数的调用
`$(func arg)`
### 字符串处理函数
#### subst
`$(subst from,to,text)`返回text中的from替换成to后的text
#### patsubst
`$(patsubst pattern,text1,text2)`查找text2中的单词是否符合pattern,符合则替换为text1,并将替换好的返回

`$(patsubst %.c,%.o,x.c.c bar.c)`结果是会返回x.c.o和bar.o
#### strip
`$(strip text)`返回去掉text中开头和结尾的空字符后的text
#### findstring
`$(findstring target,text)`会在text中寻找target,如果找到了返回text,否则返回空
去掉空格的函数

#### filter
`$(filter pattern,text)`返回符合模式的单词
#### filter-out
与filter相反，将符合模式的单词过滤掉，返回剩余的字符串

#### sort 
`$(sort text)`给text中的单词按升序排序，取出重复的单词

#### word
`$(word n ,text)`取第n个单词

#### wordlist
`$(wordlist start,end,text)`返回一个单词列表，从text的第start个单词到end个单词

#### words
`$(words text)`返回单词的个数

### 文件名操作函数
#### dir
`$(dir src/foo.c hacks)`返回参数文件的目录部分，比如示例中返回src/ ./两个目录
#### notdir
与dir相反，返回非目录部分
#### suffix
`$(suffix names)`返回文件名序列的后缀部分，如果没有后缀，则返回空
#### basename
返回文件名的前缀，包括目录的部分
#### addsuffix
`$(addsuffix .c,foo bar)`为文件添加后缀
#### addprefix
增加前缀
#### join
`$(join list1,list2)`将list1中的单词交叉的插入到list2中，第一个是list2的然后是list1的在然后是list2的以此类推
#### foreach
`$(foreach var,list,text)`将list中的单词取出放入var中然后执行text的表达式，每一次返回text表达式的值
#### if
`$(if condition,then,else)`其中else可以没有
返回被执行的那个分支的表达式
#### call
call可以用来调用其他的表达式，
```makefile
reverse = $(1)$(2)
foo = $(call reverse,a,b)
```
注意在call中提供参数的时候，第二个以及第二个之后的参数中的空格会被保留，建议输入时不要输入空格
#### origin 
`$(origin var)`他并不操作变量的值，所以在var前不需要`$`符号
origin的返回值
- undefined 变量从未被定义过
- default 变量是一个默认的定义
- environment var是环境变量
- file var 被定义在makefile里
- command line 变量是在命令行中定义的
- override 变量被override重定义过
- automatic 自动化变量

可以通过origin去判断变量的来源并对其进行修改

#### shell
`content := $(shell cat foo)`该指令会把执行命令后的输出返回，但是要注意shell函数的性能

#### error
`$(error text)`
该函数很像error函数，但是他不会让make退出，只是会输出一段警告信息

## make的运行
### make的退出码
0为成功，1为出错，2则是使用-q时且一些目标不需要更新，则返回2

### 指定makefile
`make -f name.mk`
### 指定目标
make的目标默认是第一个目标，一般来讲makefile中的第一个目标是由许多其他目标组成的

有一个环境变量叫`MAKECMDGOALS`,他会存放终极目标的列表

常见的目标：
- all 编译所有目标
- clean
- install 移动可执行文件到指定位置
- test/check 测试makefile的流程

### 检查规则
`-n --just-print --dry-run --recon` 不执行，仅仅打印指令，不管目标是否更新
`-t --touch`吧目标文件的时间更新，但是不更新目标文件，假装编译目标文件
`-q` 如果目标存在则什么也不做，否则打印出错信息
`-W file --what-if=file --assume-new=file --new-file=file` make会根据规则推导运行依赖于这个文件的命令，可以与`-n`一起使用查看某个依赖文件所发生的规则命令


### make的参数
-b -m 忽略兼容性
-B 认为所有的目标都需要更新
-C 指定读取makefile的目录，如果有多个目录，后面的路径会被认为是以前面的路径为基准的相对路径，并以最后一个作为指定目录
-debug=options 输出make的调试信息，option的取值如下
- a 所有的调试信息
- b 基本调试信息，输出不需要重新编译的目标
- v verbose,在b之上输出哪些makefile被解析，以及不需要重编译的依赖文件
- i 输出所有的隐式规则
- j 输出规则中命令的详细信息 比如pid 返回值等
- m 输出make 读取makefile 更新makefile和执行makefile的信息

-d 等于--debug=a
-e 环境变量的值覆盖makefile中同名变量
-i 忽略所有错误
-I dir 指定一个搜索makefile的目录
-j n 同时运行指令的个数
-k 出错也不停，一个目标失败后依赖他的目标也不执行
-l 指定make的负载
-o file 不重新生成名为file的目标，即使他应该被更新
-p 输出makefile中的所有数据，包括规则和变量。如果只想输出信息不想运行指令则可以使用`make -qp`，如果想查看make的预设变量和规则，可以使用该指令
-r 禁止使用隐式规则
-R 禁止使用作用于变量上的隐式规则
-s 不输出指令的输出
-S 取消-k的作用
-t 类似touch 只更新目标文件的时间
-w 输出运行make之前和之后的信息
-W file 认为file需要更新
--warn-undefined-variables 只有有未定义的变量就警告

## 9
### 隐含规则
有些隐含规则即使使用了`-r`选项一样无法避免其生效，因为很多隐含规则是使用后缀去定义的。
常见的隐含规则：
1. o文件的依赖为c文件
2. o文件自动推导为cc或.C
3. o文件依赖s文件(汇编),默认使用as汇编器
4. 没有后缀的目标依赖o文件
5. Yacc c程序的隐含规则，c文件依赖y文件

### 隐含规则使用的变量
#### 命令的变量
AR 静态库打包程序 ar
AS 汇编编译器 as
CC gcc 
CXX g++
CO co
CPP c语言的预处理器 $(CC) -E
LEX 方法分析程序lex lex
YACC 文法分析器(针对c程序) yacc

####  命令参数
在命令后面加FLAGS就成了命令的参数
### 定义模式规则
可以用模式规则定义一个隐式规则，比如目标定义要是有%,则依赖中也可以使用%.
%的展开发生在变量和函数展开之后，变量和函数产开发生在make载入时，而%展开发生在运行时

`%.o : %.c;command`

#### 自动化变量
$@ 目标文件列表
$% 仅仅当目标是函数库文件中，表示目标成员名，否则值为空
$< 第一个依赖的名字，如果依赖是使用%定义的，那么$<将是符合%的文件列表，是一个一个取出来的
$? 所有比目标新的依赖目标集合
$^ 依赖集合 会去除重复的依赖目标
$+ 依赖集合，不会去重
$* 表示目标中%之前的部分,注意如果目标的后缀可以被make的隐式规则推导，由于隐式规则是利用%实现的，所以$*会去除目标的后缀

如果需要自动化变量的目录或除去目录的部分，可以使用dir或notdir函数以及gnu的老版特性,加上D代表目录，加上F代表文件
`$(@D)`
`$(@F)`

#### 隐含规则搜索的流程
1. 搜索目标，分离目标为D和N,分别为目录和文件名
2. 创建所有符合T或N的模式规则
3. 如果模式规则有匹配所有文件的模式，就移除这个模式
4. 移除没有命令的规则
5. 执行第一个规则

#### 双后缀规则
```makefile
.c.o:
    gcc blahblah
```
`.c.o`相当于`%.o:%.c`，注意这样的规则不可以有依赖文件。
要让make知道一些特定的后缀，可以使用伪目标.SUFFIXES 对后缀进行修改
`.SUFFIXES: .mk .hk`添加两个后缀
`.SUFFIXES: `删除默认的后缀



## make更新函数库
### 库文件的成员
利用archive指定库
`archive(member)`
```makefile
foolib(hack.o):hack.o
    ar cr foolib hack.o
```

