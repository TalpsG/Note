# filesystem
## intro
(很难啊)

文件系统:
1. 抽象对人友好
2. crash safty
3. 格式化磁盘
4. 性能

文件本质上是inode的link,我可以对一个old文件创建一个new的link,然后删除old文件，这样new也可以读写文件。

## 磁盘

一个block可能对应多个sector,sector是磁盘驱动可以读写的最小单元

block0: boot sector
block1: super block 存储了磁盘等文件系统的信息
block2-31: logging
block32-44: inode
block45: bit map
block46-:data


## inode 
inode:
1. type 表明文件的类型是目录还是file
2. nlink表示文件有多少个link
3. size 文件大小
4. data部分，分为直接块和间接块

文件查找的过程 : /abc/d
1. 因为root目录是固定的，所以我们直接从root目录开始查找就可以
2. 从root的data区域读取数据到entry,然后判断name是否一样
3. 查到了abc,然后根据entry读取到abc的inode
4. 继续查abc的数据，读取到entry,判断name 是否等于d
5. 查到d，然后就可以用d的inode进行一些读写操作了

`echo hi >x` 的过程
1. 新建inode，修改内容
2. 写入数据到根目录下的数据区(entry)
3. 修改root的inode的size
4. 修改bitmap
5. 写入新文件的数据
6. 修改新文件inode


