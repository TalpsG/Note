# multithreading
## Uthread
实现用户线程
需要在uthread.c中的`thread_create()`和`thread_schedule()`中以及`thread_switch`()`添加代码。


1. create处修改ra和sp即可，注意由于栈是倒着长的，所以sp设置为stack数组的末尾处
2. schedule添加switch即可
3. thread_switch 和 swtch一样


## using threads
ph 有两个基准测试，一个是调用put 向hash table添加key,然后打印出来put的速率。
然后get 去拿key,打印本该拿到却miss的次数。


很简单，直接加锁就可以了。

## barrier
实现barrier，让线程在固定位置等待其他线程都到了barrier再继续执行。

两个问题:
1. 必须处理一连串barrier调用，每个被称作一轮，bstate.round记录轮次，当所有进程都到了barrier后要增加轮次
2. 必须处理当一个线程空转，其他线程离开barrier的情况。确保当上一轮仍在使用bstate.nthread的时候，离开barrier和空转不会增加bstate.nthread;


`pthread_cond_wait(&cond,&lock)` : 等待，并释放锁，当唤醒后会从新拿锁
`pthread_cond_broadcast(&cond,&lock)`: 唤醒所有在cond上的线程
