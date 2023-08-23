# 命令行环境
## 任务控制
### 结束进程
ctrl+c 会发送sigint给进程
ctrl+\会发送sigquit给进程
我们可以通过编写一些信号的handler使得在程序运行中可以根据信号去做出对应的处理

## 终端多路复用
tmux
## 别名
`alias alias_name="command_to_alias arg1"`

## Dotfiles
配置文件因为都有点开头，所以被叫做dotfiles

### 可移植性
可以在shell脚本中，添加一些判断针对不同的环境执行不同的指令

### 远端设备
`ssh talps@192.168.1.110` 以talps登陆对应ip的主机

### 基于密钥的认证机制
ssh会查询`.ssh/authorized_keys`来查询哪些用户可以被允许登陆，可以将对应主机的公钥拷贝到这里

### 通过ssh复制文件

ssh+tee
`cat localfile|ssh talps tee remotefile`通过管道来复制文件

### 端口转发
`ssh -L <本地端口>:<目标主机>:<目标端口> <远程主机>`
对本地端口的访问改为对目标主机的访问

### ssh配置
编写.ssh/config文件

