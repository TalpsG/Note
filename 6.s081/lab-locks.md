# lab locks
## mem allocator

目的就是改写物理内存分配器，把一个freelist变成多个freelist,每个cpu一个，使得锁的粒度更细。

## 步骤
1. 修改kmem为数组。
2. 修改kfree,把free掉的物理页挂在当前cpu对应的freelist上就行
3. 修改kalloc,优先从当前cpu对应的freelist上取物理页，没有的话就从其他的cpu的freelist上取


**kalloc里注意锁的使用，申请其他cpu的kmem的锁的时候要释放掉自己的锁**
