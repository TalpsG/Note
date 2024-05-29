# get流程
## memtable 和immtable
数据库`get`会先从`memtable`和`immtable`中查找。查找时按照`internalkey`来进行查找。
因为`memtable`和`immtable`的都是使用跳表实现的，因此查找起来很方便。如果都没找到就要去`sstable`中查找

## `sstable` 
`level0 sstable`区别于其他层，因为该层的每个表之间可能会有重叠，因此要从新到旧，一个一个查找。
在其他`level`查询时只会查一个其中的`sstable`,因为他们之间并不重叠。

1. 我们会从`current`中取出所有`level0`的`sstable`的`tablemetadata`，当中记录着`sstable`中的最大和最小`key`，通过比较来找到所有可能的`level` 0 `sstable`，并进行查找。`level0`没有就向下找`level1`的。除`level0`外其他的每一个`level`通过二分查找查询可能存放待查数据的唯一`sstable`.
2. 此后就会进到`tablecache`中查找我们需要的`sstable`，如果不在则需要将文件读进来并且将`filenumber`和`tableandfile`当作`kv`存储到`lrucache`缓存当中.
   - 缓存`sstable`时利用文件系统相关`api`得到文件大小，进而得到`footer`
   - 由`footer` 读取`indexblock` `和`filter`. 这些都存放在`tableandfile`的`table`中
3. 从`sstable`中读取`indexblock`找到对应的`index`查找`index`的过程也是二分查找，之后判断利用`filter`判断数据是否存在.
4. 在`datablock`中进行查找，按照`restart`来进行二分查找 
   - 先查`blockcache` ，返回`datablock`的迭代器，如果没查到就从文件中读取放入`cache`

其他层的`sstable`查找方式跟`level0`的一样，只不过是每个层中只需要在一张`sstable`中查找



# `lrucache`
`leveldb`中的`lrucache`实际上是分为`shard`，通过hash分成将需要缓存的数据分到多个cache中提高并发。
主要依靠三个数据结构
1. `lru_` 双向链表，包含访问过的但是现在已经没有外部引用的数据，ref为1 in_cache=true
2. `in_use_` 双向链表，包含正在使用的数据，ref大于1
3. `table_` 就是哈希表，存放数据。

在cache中查找到数据后会对该数据进行ref操作，读取数据完毕后会立刻进行unref.
`ref`: 当ref值为1,in_cache=true时，该数据在lru链表中，取出放到in_use_中。自增
`unref`: 自减，减到1时就要将数据从in_use_中移除，并放入lru链表。如果为0,则销毁该数据。

因为数据总是在两个链表之间转移，lru链表中每次都插入到链表尾部，因此头部就是最久未被使用的数据。


这里访问数据分为几个情况
1. 数据不在`lrucache`中
    - 则从磁盘中读取数据，然后放在table和in_use_中
    - 如果是读取sstable时就从磁盘读取tableandfile，然后用filenumber当作key ，`tableandfile` 指针当作v放入哈希表 .此时indexblock和filter都已经读入内存了
    - 读取datablock时，则将cache_id和datablock在文件中的offset当作key,将block指针当作value放入哈希表和in_use,
2. 数据在lrucache中如果在cache中
    - 数据在cache中，只需要ref以下增加他的引用值即可
    - 读取完数据立马unref
3. lrucache中插入数据满了
    - 将lru首部的数据从lru中删除
    - 从table中删除





明天看
1. `version`
2. `lrucache`
3. 日志恢复