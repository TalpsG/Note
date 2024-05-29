# lec9

索引并发控制

- 逻辑正确性：线程看到的数据是他应该看到的么？
- 物理正确性：数据在硬盘上存放的正确性


`lock`偏向上层概念，保护的是抽象的数据。
`latch`是底层概念，不能回滚


## latch 实现
1. 使用os的`mutex`，不过效率不高因为有睡眠和唤醒的操作。
2. 读写锁:`shared_lock`,实现在自旋锁之上。


## 哈希表并发控制
1. 对页上锁
2. 对`slot`上锁
## b+树并发控制
`crabbing/coupling`的上锁方式:
对父节点上锁后对子节点上锁，如果父节点是安全的，则释放父节点的所，然后继续向子节点的子节点要锁，一直向下。


读取时就释放父节点锁，然后要子节点锁就可以。
写时需要一直持有祖先节点的锁，直到确定写节点时不会修改祖先的节点后，才释放所有祖先节点的锁


上述情况下上的锁都是写锁，会阻塞全部的读请求。不过大部分写操作实际上并不会影响大部分的祖先节点，因此我们还有一种乐观的选择。

与上面相同，但是在向下查找写入位置的时候对祖先上的锁是读锁，只有对最后修改的节点上写锁，此时如果不影响祖先节点，自下而上释放锁即可。
否则需要从根节点从新上一边写锁。