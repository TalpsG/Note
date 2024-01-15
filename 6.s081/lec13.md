# sleep and wakeup
## 线程切换的锁
1. 获取自己的锁
2. 设置runnable
3. 进入调度器
4. 调度器释放锁

进程切换前要获取锁的目的是为了不让其他cpu核的调度器在完成切换前运行该进程。

在xv6中，不允许在执行swtch函数时，持有其他的锁，只能持有自己进程的锁。
比如a进程拿了a的锁和磁盘锁，然后切走了，b进程想用磁盘，但是会被卡在acquire里，acquire里关了中断，所以不会再调度回去，导致死锁，系统一直空转。

## lost wakeup
sleep前 while 后 被wakeup.

## 避免lost wakeup
消除窗口时间，sleep获取进程锁用来原子性的修改条件变量
wakeup需要持有两个锁来查看进程，并唤醒对应进程


## exit
exit最终会导致父进程被唤醒。
如果进程有子进程，则把子进程都给init.
进程会变成zombie状态，等待父进程回收

## kill
