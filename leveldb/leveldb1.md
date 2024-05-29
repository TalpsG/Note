# leveldb整体架构
leveldb是一个写性能十分优秀的存储引擎，是典型的LSM树(Log Structured-Merge Tree)实现。LSM树的核心思想就是放弃部分读的性能，换取最大的写入能力。


leveldb的总体架构如图所示:

![arch](https://leveldb-handbook.readthedocs.io/zh/latest/_images/leveldb_arch.jpeg) 

在进行写操作`put/delete`时，会不停的往向内存上的`memtable`写入数据。当`memtable`大于某个值的时候，就会触发`table`的切换，把当前的`mem`变成`imm`然后新建一个`mem` 。同时后台会将`imm`刷到磁盘上，生成图片上的`level0 sstable`。多个`level`的`sstable`就是经过不断刷盘`imm`和`compaction`形成的。



# memtable
`memtable`是通过跳表实现的，因此插入`put/delete`操作实际上是在跳表上插入一个新的节点。对于相同的`key`通过`internal_key`中的`seq`来区分新旧数据。置于什么是`internal_key`下面马上就会说到。

## 数据写入memtable流程
无论是通过`put/delete`还是`WriteBatch`,最终都会使用`WriteBatch`来写入数据到数据库。

通过解析`WriteBatch`的`rep_`来将`put/delete`操作写入`memtable`.
下面是`write_batch->put`操作写入`memtable`的格式，我们称之为`internal_key`

1. `key_size`:包括了后面的`keybytes`和`tag`，实际上就是`key.size()+8`
2. `keybytes`:就是`key.data()`
3. `tag`:是`seq`和`type`合并起来的，`type`用来区分`put`或`delete`,`delete`就不需要有后面的`value_size`和`value bytes`了
4. `value_size`
5. `value bytes`
在leveldb中许多长度信息的序列化后都是不定长的数据，实现变长的方式是将每一个字节的最后一位当作下一字节还是长度信息的标志，如果该字节最高位为1，则证明下一字节还是长度信息(相较于本字节7位数据的高7位数据)。

```cpp
  // Format of an entry is concatenation of:
  //  key_size     : varint32 of internal_key.size()
  //  key bytes    : char[internal_key.size()]
  //  tag          : uint64((sequence << 8) | type)
  //  value_size   : varint32 of value.size()
  //  value bytes  : char[value.size()]
```
```cpp
int InternalKeyComparator::Compare(const Slice& akey, const Slice& bkey) const {
  // Order by:
  //    increasing user key (according to user-supplied comparator)
  //    decreasing sequence number
  //    decreasing type (though sequence# should be enough to disambiguate)
  int r = user_comparator_->Compare(ExtractUserKey(akey), ExtractUserKey(bkey));
  if (r == 0) {
    const uint64_t anum = DecodeFixed64(akey.data() + akey.size() - 8);
    const uint64_t bnum = DecodeFixed64(bkey.data() + bkey.size() - 8);
    if (anum > bnum) {
      r = -1;
    } else if (anum < bnum) {
      r = +1;
    }
  }
  return r;
}
```
这里有一点需要注意，`memtable`中的`kv`顺序是按照`internal_key`中的`key`升序`seq`降序来存储的，上面的`compare`的注释里也有写到。 这样的好处下面会说！
当`memtable`的`memory_usage_`大于一个`writebuffersize`时，就会将当前的`memtable`转化为`immutable`，然后新建一个`memtable`，`imm`等待被写入磁盘。

## get流程
`db->Get(readops,key,&value)`时，会先去`memtable`和`imm`中查询。
在`memtable`中查询时，会先利用最新的`last_seq`构造一个`lookupkey`，然后在跳表中找到找到第一个大于`lookupkey`的`internal_key`。
注意第一个大于等于`lookupkey`的`internal_key`实际上是`key`部分大于等于`seq`部分小于等于的，因此我们找到了小于`last_seq`的最大序号的`internal_key`.即`memtable`中最新的记录，此时我们通过`type`来判断返回`status`是找到了还是没找到。

## Batch结构
`WriteBatch`当中实际上使用一个叫`rep_`的`string`来存储所有操作的，具体的格式如图
![batch](https://leveldb-handbook.readthedocs.io/zh/latest/_images/batch.jpeg) 
这里贴一段`write_batch.cc`中说明`rep_`格式的注释
```c
// WriteBatch::rep_ :=
//    sequence: fixed64
//    count: fixed32
//    data: record[count]
// record :=
//    kTypeValue varstring varstring         |
//    kTypeDeletion varstring
// varstring :=
//    len: varint32
//    data: uint8[len]
```

由于`WriteBatch`相当于一次修改，所以`rep`最开始是一个`seq`序号。后面的`count`是一个32位的整数，记录着`WriteBatch`里的操作个数。

我们看一下`put`的源代码
```cpp
void WriteBatch::Put(const Slice& key, const Slice& value) {
  WriteBatchInternal::SetCount(this, WriteBatchInternal::Count(this) + 1);
  rep_.push_back(static_cast<char>(kTypeValue));
  PutLengthPrefixedSlice(&rep_, key);
  PutLengthPrefixedSlice(&rep_, value);
}
```
首先修改了`count`的值，然后将操作类型添加到`rep_`末尾，最后添加`key/value`.
添加`kv`的时候会先先添加长度信息,然后添加数据信息.
```cpp
void PutLengthPrefixedSlice(std::string* dst, const Slice& value) {
  PutVarint32(dst, value.size());
  dst->append(value.data(), value.size());
}
```
`delete`操作不需要添加`value`的数据。

**leveldb-handbook上说的`WriteBatch`有维护一个总体长度我没有找到.~~不知道是不是`rep.size()`~~**

## arena 

`memtable`是通过`arena`来释放内存的
`arena`只申请空间，只有`arena`对象被销毁时才会释放所有申请的空间.
```cpp
// Allocation state
char* alloc_ptr_;
size_t alloc_bytes_remaining_;
// Array of new[] allocated memory blocks
std::vector<char*> blocks_;
```
`arena`中的`vector`保存了所有分配过的`block`的指针，用于析构函数中的内存释放。`alloc_ptr_`是当前正在使用的`block`的指针。

`arena`提供了两个对外的内存申请接口:
```cpp
// Return a pointer to a newly allocated memory block of "bytes" bytes.
char* Allocate(size_t bytes);

// Allocate memory with the normal alignment guarantees provided by malloc.
char* AllocateAligned(size_t bytes);
```
如果申请的内存大于当前块剩余空间，则要申请新的`block`.此处分为两种情况.
```cpp
if (bytes > kBlockSize / 4) {
  // Object is more than a quarter of our block size.  Allocate it separately
  // to avoid wasting too much space in leftover bytes.
  char* result = AllocateNewBlock(bytes);
  return result;
}

// We waste the remaining space in the current block.
// NOTE: yes you did
alloc_ptr_ = AllocateNewBlock(kBlockSize);
alloc_bytes_remaining_ = kBlockSize;

char* result = alloc_ptr_;
alloc_ptr_ += bytes;
alloc_bytes_remaining_ -= bytes;
return result;
```
当申请的空间大于`block`的四分之一，则申请新的块返回，在之后的内存分配中不会使用到这个块。
小于四分之一，则申请完新的块，然后把`alloc_ptr_`指过来，此后的分配在该块上进行。
# log

leveldb采用`write ahead log`日志先行的策略来防止数据因为某些`crash`而丢失.
数据库打开时会创建新的`log`文件，其实就是一个`writablefile`对象，对象内存放着`fd/pos/buffer`.其中fd就是用`open`系统调用打开的一个文件.
```cpp
Status NewWritableFile(const std::string& filename,
                        WritableFile** result) override {
  int fd = ::open(filename.c_str(),
                  O_TRUNC | O_WRONLY | O_CREAT | kOpenBaseFlags, 0644);
  if (fd < 0) {
    *result = nullptr;
    return PosixError(filename, errno);
  }

  *result = new PosixWritableFile(filename, fd);
  return Status::OK();
}
```
在`db->Write`中会先调用`addrecord`写日志，要写入日志的内容就是`WriteBatch`当中的`rep_`.

写入时，实际上是先写到内存缓冲区，日志对象中有`pos`记录当前缓冲区的位置。当要记录的内容大于缓冲区所剩空间时，则先写部分到缓冲区然后调用`write(fd....)`系统调用写入文件.如果未写入的内容小于`buffer`空间，则写入`buffer`，否则直接写到文件当中。
```cpp
Status Append(const Slice& data) override {
  size_t write_size = data.size();
  const char* write_data = data.data();

  // Fit as much as possible into buffer.
  size_t copy_size = std::min(write_size, kWritableFileBufferSize - pos_);
  std::memcpy(buf_ + pos_, write_data, copy_size);
  write_data += copy_size;
  write_size -= copy_size;
  pos_ += copy_size;
  if (write_size == 0) {
    return Status::OK();
  }

  // Can't fit in buffer, so need to do at least one write.
  Status status = FlushBuffer();
  if (!status.ok()) {
    return status;
  }

  // Small writes go to buffer, large writes are written directly.
  if (write_size < kWritableFileBufferSize) {
    std::memcpy(buf_, write_data, write_size);
    pos_ = write_size;
    return Status::OK();
  }
  return WriteUnbuffered(write_data, write_size);
}

```
## log 的结构
日志文件是分块进行存储的
每条`log`的结构如下
- 4B crc校验码
- 2B 长度 不算前7b,也就是只记录不定常数据的长度
- 1B 类型，记录该条数据是完整的日志还是日志的开头部分,中间部分或结尾部分。
- 不定长的数据

日志在文件中是分块存储的，每一个块32KB大小。如果添加日志时剩余空间不足一个`header`大小(7B)则需要填充零。

如果我现在有100KB的`WriteBatch`内容，则需要分成4个日志块才能存放下。

# sstable
`sstable`是存放在磁盘上的有序数据集合.
`sstable`是通过`compaction`生成的。
1. `minor compaction`: `immutable memtable`落到磁盘上就成了`level0 sstable`
2. `major compaction` 


`sstable`内部分为以下几部分
1. 若干`data block`
2. 若干`filter block` 存放布隆过滤器的数据 
3. `metaindex block` 记录`filter`的索引
4. `index block` 
5. `footer`
结构如图所示
![block arch](http://blog.mrcroxx.com/posts/code-reading/leveldb-made-simple/5-sstable/assets/block.svg)

`data index filter`会借助各自的`builder`来生成和写入.
`minor compaction`是会先写`datablock`，之后在按顺序写其他`block`

## datablock
`datablock`的按顺序分别有
1. 若干`kv`
2. `restarts`索引，记录了每个`restart`点在`block`中的起始位置
3. `restart`点个数
4. 5B的额外数据，包括1B的压缩类型和4B的crc校验码
`sstable`的`datablock`中是分成了许多`restart`点的。从每个`restart`点开始，每个`kv`只记录`k`与上一个`k`相同前缀的长度，不相同的后面部分的长度，以及不相同部分的数据。每个`restart`点有`block_restart_interval(16)`个`kv`.每个`restart`开始时，`shared`长度为0，同时将当前`kv`的开始位置添加到`vector`当中，当作`restart`的索引。


在`minor compaction`时，每当`datablock`超过4kb,就会将其刷盘一次，并且在下一次写入`kv`时在`indexblock`里添加一条上一个数据块的索引，.

## filter block 
当`datablock`写完后就会写`filter block`到文件.
该块主要是记录了`filter`相关的信息。
`filter block`在文件中的布局如下
1. 若干个`filter`数据块
2. 若干个`filter index`指向`filter`数据块的位置
3. 最后是一个`kfilterbaselg` 可以理解成`magicnumber`?
4. 后面还是 压缩类型 和 crc 一共5B

leveldb默认没有过滤器。

`filter blockbuilder`的成员有,`filter block`通过`builder`生成并写入文件中.
```cpp
std::string keys_;             // Flattened key contents
std::vector<size_t> start_;    // Starting index in keys_ of each key
std::string result_;           // Filter data computed so far
std::vector<Slice> tmp_keys_;  // policy_->CreateFilter() argument
std::vector<uint32_t> filter_offsets_;
```
`keys`是所有`key`。`start`记录了每个`key`在`keys`中的起始位置.
`result`是对`keys`进行布隆过滤后的数据,`filter_offsets_`则记录了每一个`filter`在`result`的起始位置。




leveldb对每`2KB`的`datablock`生成一个 `filter`. 目前在过滤器生成这里不太懂，如果当前`block_offset`很大，则根据`kFilterBase`来说要生成多个`filter`的数据。但是`GenerateFilter`一次会把所有的`key`都用掉，之后生成的`filter`就都是空的了.
我写代码调试了以下发现和我说的一样，那这里的代码是忘记改了还是怎么回事？
```cpp
void FilterBlockBuilder::StartBlock(uint64_t block_offset) {
  uint64_t filter_index = (block_offset / kFilterBase);
  assert(filter_index >= filter_offsets_.size());
  while (filter_index > filter_offsets_.size()) {
    GenerateFilter();
  }
}
void FilterBlockBuilder::GenerateFilter() {
  const size_t num_keys = start_.size();
  if (num_keys == 0) {
    // Fast path if there are no keys for this filter
    filter_offsets_.push_back(result_.size());
    return;
  }

  // Make list of keys from flattened key structure
  start_.push_back(keys_.size());  // Simplify length computation
  tmp_keys_.resize(num_keys);
  for (size_t i = 0; i < num_keys; i++) {
    const char* base = keys_.data() + start_[i];
    size_t length = start_[i + 1] - start_[i];
    tmp_keys_[i] = Slice(base, length);
  }

  // Generate filter for current set of keys and append to result_.
  filter_offsets_.push_back(result_.size());
  policy_->CreateFilter(&tmp_keys_[0], static_cast<int>(num_keys), &result_);

  tmp_keys_.clear();
  keys_.clear();
  start_.clear();
}
```
## metaindex block
该块只有一个`kv`,`key`是过滤器的名字，`value`是`filterblock`在文件中的偏移位置和大小。


## index block
`indexblock`存放着`sstable`中的数据块部分的索引。

添加索引时，实际上就是添加一条`kv`，其中`key`是大于上一个`datablock`最后一个`key`，且小于当前要写入的`key`的最短字符串(此处是为了减少索引的存储空间),`value`则是上一个块的在文件中的起始位置以及长度。
```cpp
if (r->pending_index_entry) {
  assert(r->data_block.empty());
  r->options.comparator->FindShortestSeparator(&r->last_key, key);
  std::string handle_encoding;
  r->pending_handle.EncodeTo(&handle_encoding);
  r->index_block.Add(r->last_key, Slice(handle_encoding));
  r->pending_index_entry = false;
}
```

`indexblock`和`datablock`一样，是有`restart`的，按照相同前缀和不同前缀的格式去存储的。


## footer
footer占48B,其中前几个B记录了`metaindexblock` 和`indexblock`的`offset`和`size`.之后到40B都是填充的数据，最后8B是一个`magicnumber`



