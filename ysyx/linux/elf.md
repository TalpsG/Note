# elf
ELF 头在文件总在文件的0地址处，后面跟着program header table和section header table.这两个标描述了其他的table

elf中也有调试信息

对内容分section进行权限管理
代码段可读可执行 不可写
数据段可读可写 不可执行

常见的section
.text 代码
.data 数据
.rodata
.bss 存放未初始化的数据

segment 可能包含0或多个section
section 可能不被包含在任何segment里

program header table是管理segment的
table的每个表项描述了一个segment的所有属性
