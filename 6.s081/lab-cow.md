# cow

## 描述
由于fork之后大部分情况下都会执行exec,导致copy的userspace没有使用就又丢弃了，所以可以实现copy on write的方式。当真正写对应的页面才进行复制。

## 解法
目标是推迟分配页面直到子进程写对应的地址的时候

cow fork为子进程创建页表，pte指向父进程的物理页。然后父子进程都不可以写物理页，pte都标记成不可写。当有进程写这些页面的时候，进入pagefault然后分配物理页，并进行copy，标记pte为可写

当释放物理页的时候需要注意，给最初的物理地址空间添加引用计数，当为0时再释放这些物理地址。

### 实现
计划:
1. 修改uvmcopy给子进程映射父进程的物理页，然后修改父子的pte
2. 修改usertrap 分配页面，处理pte
3. 确保所有物理页都被释放:
    - 可以为每一个物理页设置一个引用计数
    - fork时增加引用计数，pagefault里和kfree里减少引用计数
    - 计数为0 则释放物理页
4. 修改copyout ， 如果写了不让写的页就cow

### hints
1. 记录物理页有没有被cow map，可以使用RSW位去标记
2. riscv.h中有些宏可能有用
3. cow pagefault之后没有freepage了就要杀掉进程


### 踩坑过程

1. 注意修改kfree的时候要把memset放在确定释放的分支中，否则所有kfree的页面都被重置数据了。
2. 注意理清楚哪里需要增加引用
3. 注意PTE flag的设置


### 需要修改的位置

uvmcopy
kalloc
kfree
copyout
usertrap
