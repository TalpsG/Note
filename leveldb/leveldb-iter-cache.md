# lrucache
在查询过程中，`leveldb`使用了`lrucache`的方式来缓存磁盘文件如`sstable`等数据。下面我们来看看`leveldb-lrucache`是如何实现的.
## sharedLRUCache
`lrucache`是通过`ShardedLRUCache`这个类实现的，每一个`db`都会有一个`ShardedLRUCache`当作`sstable`元数据的缓存.
```cpp
class ShardedLRUCache : public Cache {
 private:
  LRUCache shard_[kNumShards];
  port::Mutex id_mutex_;
  uint64_t last_id_;
  ....
}
```
为了提高并发的处理能力，整个缓存被分成了若干个分片。通过对`key`的哈希来索引`key`归属的分片。下面看看分片内部的实现.
## LRUCache
`LRUCache`这个类中的属性如下，比较重要的有
1. `lru_` 双链表，存放读取过但当前并未被使用的数据，插入元素时永远插入到尾部，这样头部永远是最近最久未使用的元素。
2. `in_use_` 双链表，存放当前正在使用的数据
3. `table_` 就是哈希表，使用链表法处理冲突，扩容时将长度扩充至大于当前容量的最小的2的幂长度.
```cpp
// Initialized before use.
size_t capacity_;

// mutex_ protects the following state.
mutable port::Mutex mutex_;
size_t usage_ GUARDED_BY(mutex_);

// Dummy head of LRU list.
// lru.prev is newest entry, lru.next is oldest entry.
// Entries have refs==1 and in_cache==true.
LRUHandle lru_ GUARDED_BY(mutex_);

// Dummy head of in-use list.
// Entries are in use by clients, and have refs >= 2 and in_cache==true.
LRUHandle in_use_ GUARDED_BY(mutex_);

HandleTable table_ GUARDED_BY(mutex_);
```

链表中存放的是一个`LRUhandle`，当中记录了双链表的相关信息
`keyvalue`以及一个`refs`引用计数。
```cpp
void* value;
void (*deleter)(const Slice&, void* value);
LRUHandle* next_hash;
LRUHandle* next;
LRUHandle* prev;
size_t charge;  // TODO(opt): Only allow uint32_t?
size_t key_length;
bool in_cache;     // Whether entry is in the cache.
uint32_t refs;     // References, including cache reference, if present.
uint32_t hash;     // Hash of key(); used for fast sharding and comparisons
char key_data[1];  // Beginning of key
```
通过维护`ref/unref`函数来维护引用计数进而实现类似`sharedptr`的自动释放资源的功能。
`in_cache`标志数据是否在`table_`中。
```cpp
void LRUCache::Ref(LRUHandle* e) {
  if (e->refs == 1 && e->in_cache) {  // If on lru_ list, move to in_use_ list.
    LRU_Remove(e);
    LRU_Append(&in_use_, e);
  }
  e->refs++;
}

void LRUCache::Unref(LRUHandle* e) {
  assert(e->refs > 0);
  e->refs--;
  if (e->refs == 0) {  // Deallocate.
    assert(!e->in_cache);
    (*e->deleter)(e->key(), e->value);
    free(e);
  } else if (e->in_cache && e->refs == 1) {
    // No longer in use; move to lru_ list.
    LRU_Remove(e);
    LRU_Append(&lru_, e);
  }
}
```
`lru`策略主要是通过`lru_`和`in_use_`这两个双链表实现的。
我们假设以下几种场景:
1. 当缓存为空：即`table_`无数据，此时任何查询都会`miss`，然后从磁盘中取出数据构造`handle`，放入`table`和`in_use_`中，由于`handle`存放在`table`和`in_use_`中，因此使用时`refs`为2。读取数据完毕后，调用该`handle`的`unref`，如果`refs`降至1，则说明外部没有引用该数据，将其从`in_use_`中取出，放入`lru_`的尾部。

2. 缓存命中：接着上面的情况来，现在还来访问情况1的数据，此时缓存命中，将`handle`从`lru`取出放入`in_use_`. 用完拿出来放回去即可。

3. 缓存未命中，且缓存满了：此时从磁盘上取出数据后要将该数据构造成`handle`并插入到`table_`。缓存满了，需要进行驱逐，由于我们总是将`handle`添加到`lru`的尾部，因此要驱逐的元素一定在`lru`的头部，将其从`table_`和`lru`中都删除(`unref`)。如果此时缓存容量还是大于设置的值，则继续驱逐。

总结起来就是`lru`存放曾经读取过的数据，`in_use`存放正在读取的数据，`table`存放两个链表的并集。

## 应用
`leveldb`中的`sharedLRUCache`用在两个地方，一个是作为`tablecache`，另外一个是`blockcache`。顾名思义，一个作为`sstable`元数据的缓存(有`indexblock`等数据)，另一个则是作为`sstable`中的`block`的缓存(比如`datablock`).

在查找数据时会利用到这两种`cache`。等后面讲完`iterator`后会梳理一下`get`流程。

# iterator
`leveldb`中的`iterator`有很多，我们一个一个说。
## iterator
最基本的`iterator`，是一个虚类，定义了迭代器需要的方法，我们用到的`iterator`都是实现了该类。
```cpp
class LEVELDB_EXPORT Iterator {
 public:
  Iterator();

  Iterator(const Iterator&) = delete;
  Iterator& operator=(const Iterator&) = delete;

  virtual ~Iterator();

  // An iterator is either positioned at a key/value pair, or
  // not valid.  This method returns true iff the iterator is valid.
  virtual bool Valid() const = 0;

  // Position at the first key in the source.  The iterator is Valid()
  // after this call iff the source is not empty.
  virtual void SeekToFirst() = 0;

  // Position at the last key in the source.  The iterator is
  // Valid() after this call iff the source is not empty.
  virtual void SeekToLast() = 0;

  // Position at the first key in the source that is at or past target.
  // The iterator is Valid() after this call iff the source contains
  // an entry that comes at or past target.
  virtual void Seek(const Slice& target) = 0;

  // Moves to the next entry in the source.  After this call, Valid() is
  // true iff the iterator was not positioned at the last entry in the source.
  // REQUIRES: Valid()
  virtual void Next() = 0;

  // Moves to the previous entry in the source.  After this call, Valid() is
  // true iff the iterator was not positioned at the first entry in source.
  // REQUIRES: Valid()
  virtual void Prev() = 0;

  // Return the key for the current entry.  The underlying storage for
  // the returned slice is valid only until the next modification of
  // the iterator.
  // REQUIRES: Valid()
  virtual Slice key() const = 0;

  // Return the value for the current entry.  The underlying storage for
  // the returned slice is valid only until the next modification of
  // the iterator.
  // REQUIRES: Valid()
  virtual Slice value() const = 0;

  // If an error has occurred, return it.  Else return an ok status.
  virtual Status status() const = 0;

  // Clients are allowed to register function/arg1/arg2 triples that
  // will be invoked when this iterator is destroyed.
  //
  // Note that unlike all of the preceding methods, this method is
  // not abstract and therefore clients should not override it.
  using CleanupFunction = void (*)(void* arg1, void* arg2);
  void RegisterCleanup(CleanupFunction function, void* arg1, void* arg2);

 private:
  // Cleanup functions are stored in a single-linked list.
  // The list's head node is inlined in the iterator.
  struct CleanupNode {
    // True if the node is not used. Only head nodes might be unused.
    bool IsEmpty() const { return function == nullptr; }
    // Invokes the cleanup function.
    void Run() {
      assert(function != nullptr);
      (*function)(arg1, arg2);
    }

    // The head node is used if the function pointer is not null.
    CleanupFunction function;
    void* arg1;
    void* arg2;
    CleanupNode* next;
  };
  CleanupNode cleanup_head_;
};

```
## block::iter
这个`iter`专门是用在`sstable`中的`index_block`和`data_block`的。
因为这两个`block`的存储格式都是增量的，带有`restart`的。其中的`seek`方法通过二分的方式查找`keyvalue`。
```cpp
class Block::Iter : public Iterator {
 private:
  const Comparator* const comparator_;
  const char* const data_;       // underlying block contents
  uint32_t const restarts_;      // Offset of restart array (list of fixed32)
  uint32_t const num_restarts_;  // Number of uint32_t entries in restart array

  // current_ is offset in data_ of current entry.  >= restarts_ if !Valid
  uint32_t current_;
  uint32_t restart_index_;  // Index of restart block in which current_ falls
  std::string key_;
  Slice value_;
  Status status_;
  ...
}
```



对于`datablock`来说，`key`是`cacheid`和要读取的文件的`offset`。`cacheid`可以当成`sstable`的标号，缓存中同一个`sstable`中的多个`datablock`的`cacheid`是相同的。

对于`indexblock`来说，`key`是每一个索引对应的`datablock`中最后的`key`和下一个`datablock`的第一个`key`之间的最短的`key`(此举是为了减少索引的大小)，`value`是该`datablock`的`offset`和大小。


## LevelFileNumIter
在`compaction`时会使用这个迭代器，该迭代器可以按大小顺序输出同一个`level`当中的所有`sstable`的`filenumber/size`.
`key`是每个`sstable`最大的`internal_key`，`value`则是`filenumber`和`size`.


## 2level-iter
`2level-iter`是两个`iter`拼在一起的，第一个`index_iter`确定数据范围，`data_iter`用来精细的查找数据。
创建该迭代器时需要传入一个回调函数`block_function_`，该函数的功能是将`index_iter`的`value`转换为`data_iter`.
```cpp
BlockFunction block_function_;
void* arg_;
const ReadOptions options_;
Status status_;
IteratorWrapper index_iter_;
IteratorWrapper data_iter_;  // May be nullptr
// If data_iter_ is non-null, then "data_block_handle_" holds the
// "index_value" passed to block_function_ to create the data_iter_.
std::string data_block_handle_;
```
`leveldb`中有两处使用了`2level-iter`，一处是`sstable`的`iter`，`index_iter`对应`index_block`的迭代器，`data_iter_`对应`data_block`的迭代器。

另外一处是`ConcatenatingIter`。

## ConcatenatingIter
`ConcatenatingIter`的结构如下，他实际上是一个`2level-iter`套了一个`2level-iter`的迭代器。

![](~/Pictures/leveldb/leveldb-concatenatingIterator.drawio.png)
根据图来分析代码就容易了许多，下面的代码是新建`ConcatenatingIter`的代码，可以看到传入的回调函数是获取`table::iter(data_iter)`的。得到的`data_iter`本身也是一个`2level-iter`，通过`index_iter`索引`datablock`获取`data_iter`。
```cpp
Iterator* Version::NewConcatenatingIterator(const ReadOptions& options,
  // XXX:
  // block_function的作用是根据filenumber和filesize
  // 在tablecache中查找到对应的sstable并且获取他的iterator
  return NewTwoLevelIterator(
      new LevelFileNumIterator(vset_->icmp_, &files_[level]), &GetFileIterator,
      vset_->table_cache_, options);
}
```

## mergingiter
最后一个讲的是`mergingiter` 归并迭代器。大家都知道`compaction`的时候实际上是对部分`sstable`进行归并排序，生成新的`sstable`。这个迭代器就是在这个时候使用的。
```cpp
private:
// Which direction is the iterator moving?
enum Direction { kForward, kReverse };

void FindSmallest();
void FindLargest();

// We might want to use a heap in case there are lots of children.
// For now we use a simple array since we expect a very small number
// of children in leveldb.
const Comparator* comparator_;
IteratorWrapper* children_;
int n_;
IteratorWrapper* current_;
Direction direction_;
```
上面代码中就是`mergingiter`的一些成员，其中`children`是一个迭代器数组，遍历`mergingiter`的过程就是通过`FindSmallest`找到`children`迭代器中最小的`key`然后输出，`next`。通过`mergingiter`我们可以实现对`level0 sstable`的归并输出，这也是`leveldb`在`compaction`过程中做的。


## DBiter
终于到了`dbiter`，我们先看看创建`dbiter`的函数，发现在构造函数中创建了一个`NewInternalIterator`. 我们下面看看`InternalIter`。
```cpp
Iterator* DBImpl::NewIterator(const ReadOptions& options) {
  SequenceNumber latest_snapshot;
  uint32_t seed;
  Iterator* iter = NewInternalIterator(options, &latest_snapshot, &seed);
  return NewDBIterator(this, user_comparator(), iter,
                       (options.snapshot != nullptr
                            ? static_cast<const SnapshotImpl*>(options.snapshot)
                                  ->sequence_number()
                            : latest_snapshot),
                       seed);
}
```

下面就是`InternalIter`的创建函数，函数不难。可以看出实际上`InternalIter`就是一个`mergingiter`，其中包含了`memtable` ,`imm` ,`level0 sstable` 以及其他`level`的`ConcatenatingIter`，目的就是要对所有的数据进行归并。
由于`level0 sstable`可能相互重叠，所以`mergingiter`包含了所有`level0 sstable`的迭代器。对于其他`level`而言，`sstable`并不重叠，因此按顺序遍历即可`ConcatenatingIter`刚好契合我们的要求。

```cpp
Iterator* DBImpl::NewInternalIterator(const ReadOptions& options,
                                      SequenceNumber* latest_snapshot,
                                      uint32_t* seed) {
  mutex_.Lock();
  *latest_snapshot = versions_->LastSequence();

  // Collect together all needed child iterators
  std::vector<Iterator*> list;
  // XXX:mem的iter
  list.push_back(mem_->NewIterator());
  mem_->Ref();
  // XXX:imm的iter
  if (imm_ != nullptr) {
    list.push_back(imm_->NewIterator());
    imm_->Ref();
  }
  // XXX:
  // level0 中的所有sstable的iter
  // 以及后面每个level的ConcatenatingIter
  versions_->current()->AddIterators(options, &list);
  // XXX:生成归并迭代器，归并所有list中的迭代器
  Iterator* internal_iter =
      NewMergingIterator(&internal_comparator_, &list[0], list.size());
  versions_->current()->Ref();

  IterState* cleanup = new IterState(&mutex_, mem_, imm_, versions_->current());
  internal_iter->RegisterCleanup(CleanupIteratorState, cleanup, nullptr);

  *seed = ++seed_;
  mutex_.Unlock();
  return internal_iter;
}
```
下一篇文章讲解`compaction`的流程。
