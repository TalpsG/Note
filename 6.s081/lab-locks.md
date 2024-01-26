# lab locks
## mem allocator

目的就是改写物理内存分配器，把一个freelist变成多个freelist,每个cpu一个，使得锁的粒度更细。

### 步骤
1. 修改kmem为数组。
2. 修改kfree,把free掉的物理页挂在当前cpu对应的freelist上就行
3. 修改kalloc,优先从当前cpu对应的freelist上取物理页，没有的话就从其他的cpu的freelist上取


**kalloc里注意锁的使用，申请其他cpu的kmem的锁的时候要释放掉自己的锁**

## buffer cache

多进程集中使用文件系统，会争夺bcache的锁.
### 目标
修改bcache的代码，使得acquire空转的次数接近于0。
total小于500就合格了。

修改bget和brelse,使得并发的查找和释放blocks减少锁的冲突.
必须保持每一个block最多一个cache，。

将锁的名字都以bcache开头。

减少bcache的冲突比kalloc更需要技巧，因为bcachebuffer是在进程之间共享，建议使用哈希表查找block.

以下是存在锁冲突但是可以接受的情况：
1. 进程并发使用一个block, test0不会这样做。
2. 进程并发miss了，然后寻找unused的block去替换，test0不会这样做

### hints
1. 读bcache的代码
2. 使用固定大小的哈希表
3. 当buffer没找到的时候 查找和分配buffer 必须是原子的
4. 删除所有buffer的表，改为标记上次使用时间的时间戳buffer
5. bget串行化回收
6. 可能会使用两个锁，注意避免死锁
7. 替换块
8. 最后移除 bcache.lock的代码


### 实现思路

1. hash表，然后如果hash位置没有对应的buffer,就拿空闲的映射过来。
2. hash表，但是每个hash位置有多个buffer供使用，不需要拿其他空闲的buffer使用。
