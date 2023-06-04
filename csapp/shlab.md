### waitpid(pid,status,option)
WNOHANG : 如果没有子进程终止，则返回0。
WUNTRACED ：等待终止或停止的进程，返回的pid是被终止或停止的进程号
WCONTINUED ：等待正在运行的进程的终止，或者被停止进程受到SIGCONT信号重新开始

### kill(pid,sig)
pid>0:发给pid进程信号sig
pid<0:发给组号为pid进程组中进程sig  
pid=0:发给调用kill的进程所在进程组中的全部进程sig

### setpgid(pid,pgid)
pid = 0:将组id设为本pid 
pgid = 0:将组id设置为pid

# tips
1. eval 中父进程需要在fork之前调用sigprocmask block SIGCHILD信号，在add job后unblock这些信号,在子进程继承了 blocked vector之后需要在execve之前unblock 信号
2. ctrl-c发送给前台所有进程SIGINT信号
3. 


### exec函数组
程序开头注册的sig handler函数 在execv函数执行其他程序后就失效了。

### 进程组id
fork出的进程如果不设置组id默认与父进程一个组

### git feature 
