# shell 脚本
变量展开需要$符号
当打印字符串时，`""`内的变量会被展开，而单引号不会。

变量赋值时不要有空格
数字变量相加需要这样`int=$((num1+num2))`

```shellscript
mcd (){
    mkdir -p "$1"
    cd "$1"
}
```
$0-9 命令行中的参数，0是脚本名称，其他就是第几个参数
$@ 所有参数
$# 参数个数
$? 上一个程序的返回码
$$ 当前脚本的pid
!! 执行上一条指令
$_ 上一条命令的最后一个参数

返回码可以和逻辑操作一起使用，进行判断，执行对应的指令
`true||echo "hello"`


可以使用$(CMD)来获取CMD的输出
比如


进程替换
`diff <(ls foo) <(ls bar)`其中<(CMD)会执行指令并将其输出存入一个临时文件中，并用文件名替换该段代码


```shellscript
for file in $(ls)
    echo $(file)
```

## 通配符
？匹配一个字符
*任意多个字符
{}可以用于自动展开 例如`image.{png,jpg}`等价于`image.png image.jpg`

## shebang
`#!/usr/local/bin/python`
第一行的注释可以说明该脚本需要使用的解释器

`#!/usr/bin/env python`
可以使用env命令来确定解释器，env命令利用环境变量来定位解释器的位置

## shell函数和脚本的不同
shell函数只能使用与shell相同的语言，而脚本可以使用任意语言
函数在定义时被家在，脚本则是执行时才加载
函数会在当前的shell环境执行，可以改变当前的目录，而脚本不可以，脚本需要使用export到处环境变量，将值传递给环境变量

## 查找文件
find命令
`find . -name src -type d `查找所有名为src的文件夹
`find . -path '*/test/*.py'-type f `查找符合模式的(名为test.py)的文件
`find . -mtime -1` 查找前一天修改的文件
同时find也可以执行一些简单的操作
`find . -name '*.tmp' -exec rm{}'
`find . -name '*.png' -exec convert{} {}.jpg'

-iname选项可以不区分名字的大小写

## 查找代码
grep
-C n 获取结果的上下文前后n行
-v 获取结果的反选

## 查找shell命令
history可以查看历史命令
ctrl+r可以搜索历史命令
在输入指令前加一个空格，该条指令就不会进入shell记录中

## 文件夹导航
autojump

## 课后练习
1. ls 
    - 所有文件 `ls -a`
    - 文件输出格式使用人可以理解的方式`ls -h -l`-h是human readable的意思
    - ls以最近访问顺序排序 `ls -u -lt` 如果以修改顺序则是`ls -c -lt`
2. marco记录当前的目录，polo返回记录的目录，写出这两个函数
```shellscript
record(){
        pwd > ~/.marco
}
load(){
        cd "$(cat ~/.marco)"
}
```
3. 有一个很少出错的程序，编写一个脚本执行这个程序直到出错，并捕获他的标准输出和标准错误流到文件，最后输出这些内容，加分项，显示此前执行的次数。

```shellscript
int=0
content=$(./test.sh)    
ret=$?                  
while ((ret == 0))      
do                      
        int=$((int+1))
        content=$(./test.sh)
        ret=$?
done
echo "$content" > error.txt
echo "$int times success" >> error.txt
cat error.txt
```

### xargs
xargs可以从标准输出中获取参数并传递给其他命令

`cmd|xagrs opt cmd`

xargs的选项
-I 用于指定替换字符串，将输入中的每一行替换为命令中的字符串
-n 指定参数的数量
-d 指定输入的分隔符


