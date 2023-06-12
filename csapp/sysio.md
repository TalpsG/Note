# system io
## unix io view
早期的unix与其他系统不同，不区分文件的类型，仅仅把文件当作字节流。

### 文件类型
regular file:arbitrary data
directory:index of a related group files
socket :for communicating with a process on another machine
pipe
symbolic links
block and character devices

**perror**
检查系统调用返回值，perror打印系统调用的error

### read调用
read返回值
0：已经读到eof或网络连接关闭了
>0:正确
<0:错误

## RIO

## file
### metadata

### sharefiles

父进程fork出了子进程，父进程和子进程共享了打开文件表，因为他们的文件描述符fd的表是一样的，文件描述符指向了同一个打开文件表，当任意进程读过之后，另外一个进程再进行读时都会从新的位置开始读。 

### redirection
```c
dup2();
```



### sum
1. 面向文本的IO函数不可以对二进制文件使用，可以使用rio函数代替
2. 字符串函数不能对二进制文件使用