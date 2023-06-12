#system io
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
