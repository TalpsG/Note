# lab 9 fs
## 大文件
### 目标
为inode添加一个二级间接块，增大文件的最大容量
### 步骤
1. bmap用于返回文件逻辑块号对应的物理块号，添加检索二级间接块的代码
2. itrunc用于截断文件，回收文件的所有块，添加回收二级块的部分即可

### 注意
修改磁盘上的块(尤其是间接索引块)都要用log_write登记到log里面，用于最后的事务提交



## symbolic links
在xv6中添加符号链接

### hints

1. 添加一个syslink的系统调用
2. 添加新的文件类型 T_SYMLINK
3. 添加新的标志位 O_NOFOLLOW
4. `symlink(target,path)`在path处创建一个指向target的symlink,target可以不存在。在symlink inode的data里面应该存放着target的路径。
5. 修改open的实现。
6. 如果symlink链接的还是symlink,则要递归的打开symlink
7. 除了open 其他syscall不得跟随symlink


### 步骤
1. 添加sys_symlink系统调用，新建inode,向inode数据内写入链接的target
2. 修改sys_open ，打开symlink文件时打开的是target

### 注意事项
1. 注意锁的使用
2. 注意symlink函数仅仅创建了链接，并没有打开，所以symlink最后要用`iunlockput`解锁inode,因为仅仅是建立symlink而非打开，所以要put回去

nlink的意思就是指向该文件的链接，创建inode之后文件默认必须有一个链接，然后当链接为0的时候(unlink)，文件就会被删除。
