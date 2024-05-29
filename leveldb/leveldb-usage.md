# 环境搭建

## 编译leveldb静态库
下面的环境搭建适用于`archlinux`.如果是其他发行版本也可参考一下.
`snappy`库在`ubuntu`下可以使用源码编译安装或者`apt`安装`libsnappy-dev`
```bash
git clone https://github.com/google/leveldb.git
cd leveldb
```
在leveldb目录下打开`CMakeLists`文件，更改第75行左右的代码
```cmake
# Disable RTTI.
string(REGEX REPLACE "-frtti" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
```
注释掉上面这行set，因为我们之后要继承`leveldb::Comparator` 需要打开`rtti` .
之后执行下面的代码

```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .
```
上述指令执行过后在`leveldb/build`下会有一个`libleveldb.a`，我们就用这个静态库来链接我们自己写的程序.



## 新建自己的工程
在一个你喜欢的位置新建文件夹`your-project-name`，然后进入`your-project-name`，
我们的文件夹下的文件有:
```ascii
❯ tree .
.
├── build   # 这个是目录
├── CMakeLists.txt
└── test01.cc
```
`test01.cc`就是咱们的源文件，后面可以写多个许多`.cc`文件，每一个文件都可以有main函数.
因此在后面的`CMakeLists`中要为每一个编写`add_executable`和`target_link_libraries`.

下面为我们的工程编写一个`CMakeLists.txt`文件。
将下面的内容复制到`CMakeLists`中，要按照你的目录位置修改其中的一些路径，路径建议写绝对路径.
`CMakeLists`有感觉不明白的可以交给gpt翻译一下
```cmake
cmake_minimum_required(VERSION 3.16)
project("leveldb-talps") # 此处请更换成你的项目名称

# c++20  为了使用std::format 需要较新的编译器支持 不过不使用也行
# 所以这两行set没有也没事，下面的std::format可以通过其他方式实现,就是效率低了好多
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# header
include_directories("path/to/leveldb/include") # 此处的路径是填写leveldb下的include文件夹的路径

# leveldb
add_library(libleveldb STATIC IMPORTED)
set_target_properties(libleveldb PROPERTIES
    IMPORTED_LOCATION "path/to/libleveldb.a"  # 此处路径填写libleveldb.a的路径，后面是leveldb/build/libleveldb.a
)

add_executable(test01 ./test01.cc)                # 此处的test是指你的executable 的名字也就是我们之后make target的target.
                                              # 后面跟上cc文件
target_link_libraries(test01 libleveldb snappy) # leveldb用到了snappy,这一个google开源的压缩解压缩数据的库，我们就写snappy就可以 
                                              # 如果本地没安装过需要安装一下,具体可以去snappy的github主页，里面有安装的教程

# 之后可以编写多个add_executable 和target_link_libraries
# 比如下面的test02
# add_executable(test02 ./test02.cc)            
# target_link_libraries(test02 libleveldb snappy) 
```


接下来在`CMakeLists`所在目录下编写`test01.cc` ,代码如下
```cpp
#include <iostream>

#include "leveldb/db.h"
#include "leveldb/options.h"
#include "leveldb/status.h"
int main(int argc, char* argv[]) {
  leveldb::Options opts;
  opts.create_if_missing = true;
  leveldb::DB* db = nullptr;
  leveldb::Status status = leveldb::DB::Open(opts, "./talps-test01", &db);
  if (status.ok()) {
    std::cout << "leveldb open success\n";
  } else {
    std::cout << "leveldb open fail" << status.ToString() << "\n";
  }
  if (db) {
    delete db;
  }
  return 0;
}
```
这段代码很简单，就是打开或者创建一个数据库，然后输出成功或者失败。


在我们新建工程内的`CMakeLists`所在目录下
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
# 如果需要compile_commands.json 则使用下面的指令
# cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Debug
make test01
```
此时就会编译test这个target，编译成功后执行即可。
```ascii
❯ ./test01
leveldb open success
```


# 创建db
创建leveldb的方式如下代码
```cpp
leveldb::Options opts;          // 打开数据库的控制信息，具体内容请看源码
opts.create_if_missing = true;  // 数据库不存在则新建
leveldb::DB* dbptr = nullptr;
leveldb::Status state = leveldb::DB::Open(opts, "db", &dbptr); // 会在当前目录下新建db文件夹，然后内部存放数据文件
if (!s.ok()) {
  std::cout << "leveldb open fail : " << s.ToString() << "\n";
  return -1;
}
```
此后判断`state.ok()`，真则打开成功.

打开失败的原因有很多这里举几个例子
- `Open`时`opts`中的`comparator`和已有数据库的比较器不同名

# put/delete/get/write_batch
`DB`对象有`put`、`delete`和`get`方法，因此我们直接调用即可
```cpp
// 此处的write_opts表示了写操作的一些控制信息
// 比如下面的sync，为真时每次写入都会去写磁盘
// 详细内容可以去看注释
leveldb::WriteOptions write_opts;
leveldb::ReadOptions read_ops;
write_opts.sync = false;
// 普通的put
db->Put(w_opts,"makima","makima");
db->Put(w_opts,"haruki","haruki");
// 普通的delete
db->Delete(w_opts,"makima");
// 普通的get
std::string value;
db->Get(read_ops, "haruki", &value);
// writebatch
```


leveldb也提供了`WriteBatch`用于批量的进行写操作，好处我gpt了一下，分为这几点
- 性能提升：`WriteBatch` 通过减少磁盘同步操作的次数来优化写入性能。多个写入操作可以被组合成一个批处理操作，这样它们就可以在一次磁盘同步中一起写入，从而降低了I/O开销。
- 原子性：`WriteBatch` 保证了所有的写入操作要么全部成功，要么全部失败。这对于保持数据库状态的一致性非常重要，特别是在发生错误或崩溃时。
- 减少写放大：在某些存储系统中，尤其是使用了日志结构合并树(LSM-tree)的系统如 LevelDB，小的写入操作会导致写放大（写入的数据量比实际数据要大）。`WriteBatch` 通过批量处理减少了这种情况的发生。
- 简化错误处理：因为 `WriteBatch` 是一个原子操作，所以用户只需要处理一个操作的成功或失败，而不是必须为每一个单独的写入操作处理错误。
- 并发控制：`WriteBatch` 可以帮助更简单地实现并发控制，因为它将多个操作捆绑为一个单独的原子操作，这减少了并发操作可能导致的复杂性。

值得一提的是，`put/delete`实际上也是使用了`WriteBatch`实现的，只不过`WriteBatch`中仅有一条记录而已.

```cpp
leveldb::WriteBatch wb;
wb.Put("kazusa", "kazusa");
wb.Put("setsuta", "setsuna");
wb.Delete("kazusa");
db->Write(w_opts, &wb);

```
使用`writebatch`后，批量的操作会被原子的写入数据库。

# iterator
数据提供了迭代器的相关方法，我们可以使用迭代器来对数据进行顺序扫描。
```cpp
std::unique_ptr<leveldb::Iterator> iter(db->NewIterator(r_opts));
iter->Seek("b"); 
// iter->SeekToLast();
// iter->SeekToFirst();
while (iter->Valid()) {
  auto key = iter->key().ToString();
  auto value = iter->value().ToString();
  std::cout << key << " : " << value << "\n";
  iter->Next();
}
```
这里要注意的是`iter`创建过后需要主动`seek`一下，`Seek(Slice)`这个函数会找到第一个字典序大于等于参数的位置。

# 自定义比较器
在我们之前的示例代码中，我们使用的用于打开数据库的`leveldb::Options`对象都是默认构造生成的，在其构造函数中:

`Options::Options() : comparator(BytewiseComparator()), env(Env::Default()) {}`

`comparator`默认是按`key`的字节比较的大小。我们也可以自己编写一个`comparator`。

```cpp
class TalpsComparator : public leveldb::Comparator {
 public:
  using Slice = leveldb::Slice;
  auto Compare(const Slice& a, const Slice& b) const -> int override {
    return b.compare(a);
  }
  virtual const char* Name() const override { return "TalpsComparator"; }
  virtual void FindShortestSeparator(std::string* start,
                                     const Slice& limit) const override {}
  virtual void FindShortSuccessor(std::string* key) const override {}
};
```
自定义的比较器需要自己继承`leveldb::Comparator`，实现其纯虚函数即可。上述代码实现了一个与默认比较器相反的降序比较器。
后两个函数我还不知道有啥用，但是不影响我们现在的功能，因此给出空实现即可。

# snapshot 快照
现在假设数据库中存储着这些数据
| key   | value    |
|--------------- | --------------- |
| setsuna   | setsuna   |
| kazusa   | kazusa   |

我们有两个线程，一个线程执行交换`value`的操作
```cpp
auto task1 = [&] {
  leveldb::WriteBatch wb;
  wb.Put(touma, ogiso);
  wb.Put(ogiso, touma);
  db->Write(write_ops, &wb);
};
```
另外一个进程查这两个`value`然后过一会再查一次
```cpp
auto task2 = [&] {
  std::string value;
  leveldb::ReadOptions read_ops;
  db->Get(read_ops, touma, &value);
  std::cout << "touma : " << value << "\n";
  db->Get(read_ops, ogiso, &value);
  std::cout << "ogiso : " << value << "\n\n";

  std::this_thread::sleep_for(std::chrono::seconds(1));

  db->Get(read_ops, touma, &value);
  std::cout << "touma : " << value << "\n";
  db->Get(read_ops, ogiso, &value);
  std::cout << "ogiso : " << value << "\n";
};
```
此时可能的结果是
```ascii
情况1
❯ ./test07
touma : setsuna
ogiso : kazusa

touma : setsuna
ogiso : kazusa


情况2
touma : kazusa
ogiso : kazusa

touma : setsuna
ogiso : kazusa
```
第一种情况得到了批量写的中间操作，且两次查询结果不一样。第二种两次查询结果不一样。

可以使用`snapshot`来解决这种问题。

```cpp
auto task2 = [&] {
  std::string value;
  leveldb::ReadOptions read_ops;

  // 新增代码
  std::shared_ptr<const leveldb::Snapshot> snapshot_ptr(
      db->GetSnapshot(),
      [&](const leveldb::Snapshot* p) -> void { db->ReleaseSnapshot(p); });
  read_ops.snapshot = snapshot_ptr.get();
  // 新增代码结束

  db->Get(read_ops, touma, &value);
  std::cout << "touma : " << value << "\n";
  db->Get(read_ops, ogiso, &value);
  std::cout << "ogiso : " << value << "\n\n";

  std::this_thread::sleep_for(std::chrono::seconds(1));

  db->Get(read_ops, touma, &value);
  std::cout << "touma : " << value << "\n";
  db->Get(read_ops, ogiso, &value);
  std::cout << "ogiso : " << value << "\n";
};
```
在`readoptions`对象中使用快照，此时查询都会在我们指定的快照里进行，就避免了数据不一致的现象。

**需要注意的是快照的析构函数是`protected`的，无法通过`delete`将其释放掉。这里要使用`db`对象的`ReleaseSnapshot`这个方法来进行释放资源。**
上述代码使用了自定义删除器的`shared_ptr`来进行自动释放快照。


# compaction
这块的原理我也不太懂，就不误导大家了。感兴趣的可以看这个链接 [leveldb-handbook](https://leveldb-handbook.readthedocs.io/zh/latest/ "leveldb-handbook") ,以及源码和网络上的讲解视频来学习。

compaction大致的意思就是落盘的数据实际上是有很多冗余数据的，compaction的目的就是将冗余的数据删除。
后台会有线程自动的进行compaction,但是我们也可以手动compaction.

下面这段注释是`db`中的`CompactRange`方法的注释
```cpp
// Compact the underlying storage for the key range [*begin,*end].
// In particular, deleted and overwritten versions are discarded,
// and the data is rearranged to reduce the cost of operations
// needed to access the data.  This operation should typically only
// be invoked by users who understand the underlying implementation.
//
// begin==nullptr is treated as a key before all keys in the database.
// end==nullptr is treated as a key after all keys in the database.
// Therefore the following call will compact the entire database:
//    db->CompactRange(nullptr, nullptr);

// 压缩键范围 [*begin,*end] 下的底层存储。
// 特别是，删除和覆盖的版本将被丢弃，
// 并且数据被重新排列以减少访问数据所需操作的成本。
// 这个操作通常只应由理解底层实现的用户调用。
// begin==nullptr 被视为数据库中所有键之前的键。
// end==nullptr 被视为数据库中所有键之后的键。
// 因此，以下调用将压缩整个数据库：
//    db->CompactRange(nullptr, nullptr);

virtual void CompactRange(const Slice* begin, const Slice* end) = 0;
```
下面的代码插入了大量数据后删除，最后手动compaction。
我们可以gdb调试一下然后在插入完毕，删除完毕和compaction完毕三个地方查看`db-compaction`的大小。
```cpp
std::string getstring() {
  std::string str;
  int num = 999983;
  int add = 995023;
  for (int i = 0; i < 1024; i++) {
    str.push_back(((num % (i + 4657) >> 5) + add) % 26 + 0x61);
  }
  return str;
}
int main(int argc, char* argv[]) {
  leveldb::DB* db;
  leveldb::Options ops;
  ops.create_if_missing = true;
  auto s = leveldb::DB::Open(ops, "db-compaction", &db);
  if (!s.ok()) {
    std::cout << "db-compaction open fail : " << s.ToString() << "\n";
    return -1;
  }
  leveldb::WriteOptions write_ops;
  std::unique_ptr<leveldb::DB> dbptr(db);
  for (int i = 0; i < 500000; i++) {
    // 此处需要c++ 20支持
    leveldb::Slice k = std::format("userinfo:{}", i); 
    leveldb::Slice v = getstring();
    db->Put(write_ops, k, v);
  }

  std::cout << "delete\n";

  for (int i = 0; i < 500000; i++) {
    // 此处需要c++20  支持
    leveldb::Slice k = std::format("userinfo:{}", i);
    db->Delete(write_ops, k);
  }

  std::cout << "compaction\n";
  db->CompactRange(nullptr, nullptr);
  return 0;
}
```

我测试了许多次，下面的数据是效果最明显的一次，其他的效果都是。下面的数据分别是添加数据后，删除数据后，以及compaction后`db-compaction`的大小.
```ascii
❯ du -sh .
40M     .
❯ du -sh .
42M     .
❯ du -sh .
84K     .
```
各位也可以在自己电脑上尝试一下。

