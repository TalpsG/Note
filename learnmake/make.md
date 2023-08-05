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
`%.o : var = value`
