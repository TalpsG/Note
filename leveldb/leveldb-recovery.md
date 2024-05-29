# 版本控制
`leveldb`的版本控制是通过`manifest`，`current`文件以及`versionset`，`VersionEdit`和`version`这几个类一起实现的。

我们先说说`version`相关的。
`leveldb`中版本就是`version`对象，`versionset`就是版本的集合。`leveldb`通过维护多个版本来实现写和读的并发(支持并发的一写多读)。
`leveldb`将`compaction`视为`version`的更新，当有`sstable`的变化时，就会产生新的`version`。而`versionedit`实际上就是`version`之间的变化。`version + versionedit = new_version`。产生新的`version`这一个过程实际上就分为两步:
1. 生成`versionedit`
2. 将`versionedit`应用到当前版本生成`new_version`

## versionset
`versionset`的部分成员如下，其中`current`指向了当前的版本(其实就是最新版本)。`versionset`将所有活着的`version`都放入了一个双链表中，`dummy`是链表的空的头节点。

`next_file_number_`记录着下一个输出到磁盘的文件的编号，这个编号是许多类文件共享的，`manifest` `log`和`sstable`的序号都是使用`next_file_number_`生成的。
```cpp
Env* const env_;
const std::string dbname_;
const Options* const options_;
TableCache* const table_cache_;
const InternalKeyComparator icmp_;
uint64_t next_file_number_;
uint64_t manifest_file_number_;
uint64_t last_sequence_;
uint64_t log_number_;
uint64_t prev_log_number_;  // 0 or backing store for memtable being compacted

// Opened lazily
WritableFile* descriptor_file_;
log::Writer* descriptor_log_;
Version dummy_versions_;  // Head of circular doubly-linked list of versions.
Version* current_;        // == dummy_versions_.prev_

// Per-level key at which the next compaction at that level should start.
// Either an empty string, or a valid InternalKey.
std::string compact_pointer_[config::kNumLevels];
```
## version
`version`的部分成员如下.
每一个版本有一个`ref`引用计数，在读写时都要先`ref`一下，增加引用计数。使用完毕`unref`减少引用计数，当引用计数为0时，`unref`中会自动将没有人使用的`version`销毁.

`files`则记录着该版本中各个`level`的`sstable`文件元数据，包括文件号，文件大小，最大最小`InternalKey`等。

由于多版本的机制，在`compaction`输出新的`sstable`后，老`sstable`文件并不能直接删除，因为当前可能还有旧的版本还在使用该文件。

```cpp
VersionSet* vset_;  // VersionSet to which this Version belongs
Version* next_;     // Next version in linked list
Version* prev_;     // Previous version in linked list
int refs_;          // Number of live refs to this version

// List of files per level
std::vector<FileMetaData*> files_[config::kNumLevels];

// Next file to compact based on seek stats.
FileMetaData* file_to_compact_;
int file_to_compact_level_;

// Level that should be compacted next and its compaction score.
// Score < 1 means compaction is not strictly needed.  These fields
// are initialized by Finalize().
double compaction_score_;
int compaction_level_;
```
## versionedit
`versionedit`是在`compaction`发生过程中生成的，记录了`compaction`后的差别，比如`newfile`和`deletefile`就记录了新版本中要添加和删除的`sstable`文件。

在`compaction`后就会生成`versionedit`，并且将`versionedit`应用到当前版本上生成新的版本。
```cpp
std::string comparator_;
uint64_t log_number_;
uint64_t prev_log_number_;
uint64_t next_file_number_;
SequenceNumber last_sequence_;
bool has_comparator_;
bool has_log_number_;
bool has_prev_log_number_;
bool has_next_file_number_;
bool has_last_sequence_;

std::vector<std::pair<int, InternalKey>> compact_pointers_;
DeletedFileSet deleted_files_;
std::vector<std::pair<int, FileMetaData>> new_files_;
```

## manifest
`manifest`可以看作是`versionedit`的日志，记录数据的格式和日志格式一样，分为`start/mid/end/full`等类型。只不过`payload`换成了`versionedit`序列化后的数据，一个`versionedit`对应一整条日志(`manifest`最开始有一个`version`的快照)。
当数据库宕机重启后，我们可以通过`manifest`文件来恢复到`manifest`记录的最新版本上。

`manifest`文件这里我本来以为会有多个`manifest`文件，但是我查找源码后发现一个数据库实例好像只有一个`manifest`文件。恢复时会判断是否能复用磁盘上的`manifest`，不能的话恢复后就会新建一个`manifest`文件。每个`manifest`文件会一直用到数据库关闭。


## current
`current` 就更简单了，仅仅存放当前的`manifest`文件的文件名. 用于数据库恢复时找到对应的`manifest`文件进行版本的恢复.

# recovery

## 大致流程
当我们调用`open`打开数据库时，在`open`内部会调用`recover`来进行所有的恢复工作。恢复之后会进行一些后续的工作比如新建`memtable`等，不过这不是本文重点。

`recover`函数中会通过`manifest`和`log`两种方式来进行恢复。

通过日志来恢复的方式很简单，只需要读取本地数据库目录中的log文件并按照日志写入`memtable`即可.

这里有一个`save_manifest`参数一直从`open`传到了`versions->recover`和`RecoverLogFile`里，该变量会被这两个函数设置，最后进行判断，他的作用主要分成两点。
1. 通过`manifest`恢复文件后的复用`manifest`文件
  - 不能复用则为`true`
  - 在`ReuseManifest`的函数中不能复用不会设置`versionset`的`descriptor_log_/file_`，这些属性初值为`nullptr`，在写`manifest`才根据`next_file_number_`新建。
2. 日志恢复时如果写磁盘则会导致版本更新
  - 版本更新导致需要写`edit`到`manifest`

在恢复完毕后，会判断`save_manifest`，真则需要调用`logandapply`写`manifest`文件。无论是上述哪种情况，该变量都会设置`true`:
1. 不能复用`manifest`文件则需要新建`manifest`文件，`logandapply`中如果`descriptor`相关的属性为`nullptr`则会新建`manifest`文件并填入
2. 如果版本更新则需要将`edit`记录到`manifest`文件当中。

```cpp
Status DBImpl::Recover(VersionEdit* edit, bool* save_manifest) {
  mutex_.AssertHeld();
  // Ignore error from CreateDir since the creation of the DB is
  // committed only when the descriptor is created, and this directory
  // may already exist from a previous failed creation attempt.
  env_->CreateDir(dbname_);
  assert(db_lock_ == nullptr);
  Status s = env_->LockFile(LockFileName(dbname_), &db_lock_);
  if (!s.ok()) {
    return s;
  }

  if (!env_->FileExists(CurrentFileName(dbname_))) {
    if (options_.create_if_missing) {
      Log(options_.info_log, "Creating DB %s since it was missing.",
          dbname_.c_str());
      s = NewDB();
      if (!s.ok()) {
        return s;
      }
    } else {
      return Status::InvalidArgument(
          dbname_, "does not exist (create_if_missing is false)");
    }
  } else {
    if (options_.error_if_exists) {
      return Status::InvalidArgument(dbname_,
                                     "exists (error_if_exists is true)");
    }
  }

  // XXX:恢复直接作用于versions上了
  // 当前versions只有一个version
  s = versions_->Recover(save_manifest);
  if (!s.ok()) {
    return s;
  }
  SequenceNumber max_sequence(0);

  // Recover from all newer log files than the ones named in the
  // descriptor (new log files may have been added by the previous
  // incarnation without registering them in the descriptor).
  //
  // Note that PrevLogNumber() is no longer used, but we pay
  // attention to it in case we are recovering a database
  // produced by an older version of leveldb.
  const uint64_t min_log = versions_->LogNumber();
  const uint64_t prev_log = versions_->PrevLogNumber();
  std::vector<std::string> filenames;
  // XXX:获取数据库目录下的所有文件
  s = env_->GetChildren(dbname_, &filenames);
  if (!s.ok()) {
    return s;
  }
  // XXX:获取所有活跃的文件(有版本引用的文件)
  std::set<uint64_t> expected;
  versions_->AddLiveFiles(&expected);
  uint64_t number;
  FileType type;
  std::vector<uint64_t> logs;
  for (size_t i = 0; i < filenames.size(); i++) {
    if (ParseFileName(filenames[i], &number, &type)) {
      expected.erase(number);
      if (type == kLogFile && ((number >= min_log) || (number == prev_log)))
        logs.push_back(number);
    }
  }
  // XXX:活跃文件却不在目录下则缺失文件。
  if (!expected.empty()) {
    char buf[50];
    std::snprintf(buf, sizeof(buf), "%d missing files; e.g.",
                  static_cast<int>(expected.size()));
    return Status::Corruption(buf, TableFileName(dbname_, *(expected.begin())));
  }

  // Recover in the order in which the logs were generated
  std::sort(logs.begin(), logs.end());
  for (size_t i = 0; i < logs.size(); i++) {
    // XXX:从日志恢复
    s = RecoverLogFile(logs[i], (i == logs.size() - 1), save_manifest, edit,
                       &max_sequence);
    if (!s.ok()) {
      return s;
    }

    // The previous incarnation may not have written any MANIFEST
    // records after allocating this log number.  So we manually
    // update the file number allocation counter in VersionSet.
    versions_->MarkFileNumberUsed(logs[i]);
  }

  if (versions_->LastSequence() < max_sequence) {
    versions_->SetLastSequence(max_sequence);
  }

  return Status::OK();
}

```
## manifest recovery
在`versions_->recover`中使用了`manifest`文件来进行数据库的恢复工作。
在该函数中，我们先要寻找`current`文件，再通过`current`文件找到`manifest`文件。此后的恢复工作就很简单了，日志的格式大家都熟悉，上一篇文章介绍过。将`manifest`文件中的`edit`一条一条的读取出来，然后从空的`version`一步一步恢复成数据库关闭前的最新`version`(`manifest`文件前面一部分其实是一个`version`的快照，后面才是一条一条的`versionedit`)。最后`apply`和`saveto`生成新的版本，并添加到`versionset`当中。


```cpp
Status VersionSet::Recover(bool* save_manifest) {
  struct LogReporter : public log::Reader::Reporter {
    Status* status;
    void Corruption(size_t bytes, const Status& s) override {
      if (this->status->ok()) *this->status = s;
    }
  };

  // Read "CURRENT" file, which contains a pointer to the current manifest file
  // XXX:Current文件内仅仅记录了manifest的文件名
  std::string current;
  Status s = ReadFileToString(env_, CurrentFileName(dbname_), &current);
  if (!s.ok()) {
    return s;
  }
  if (current.empty() || current[current.size() - 1] != '\n') {
    return Status::Corruption("CURRENT file does not end with newline");
  }
  current.resize(current.size() - 1);

  std::string dscname = dbname_ + "/" + current;
  SequentialFile* file;
  // XXX:读取manifest文件
  s = env_->NewSequentialFile(dscname, &file);
  if (!s.ok()) {
    if (s.IsNotFound()) {
      return Status::Corruption("CURRENT points to a non-existent file",
                                s.ToString());
    }
    return s;
  }

  bool have_log_number = false;
  bool have_prev_log_number = false;
  bool have_next_file = false;
  bool have_last_sequence = false;
  uint64_t next_file = 0;
  uint64_t last_sequence = 0;
  uint64_t log_number = 0;
  uint64_t prev_log_number = 0;
  // XXX:将current version放入builder中，此后会利用builder来恢复current文件
  Builder builder(this, current_);
  int read_records = 0;

  {
    LogReporter reporter;
    reporter.status = &s;
    // 读取current文件
    log::Reader reader(file, &reporter, true /*checksum*/,
                       0 /*initial_offset*/);
    Slice record;
    std::string scratch;
    // 读取manifest当中的edit
    while (reader.ReadRecord(&record, &scratch) && s.ok()) {
      ++read_records;
      VersionEdit edit;
      s = edit.DecodeFrom(record);
      if (s.ok()) {
        if (edit.has_comparator_ &&
            edit.comparator_ != icmp_.user_comparator()->Name()) {
          s = Status::InvalidArgument(
              edit.comparator_ + " does not match existing comparator ",
              icmp_.user_comparator()->Name());
        }
      }

      if (s.ok()) {
        // XXX:记录到builder中
        builder.Apply(&edit);
      }

      if (edit.has_log_number_) {
        log_number = edit.log_number_;
        have_log_number = true;
      }

      if (edit.has_prev_log_number_) {
        prev_log_number = edit.prev_log_number_;
        have_prev_log_number = true;
      }

      if (edit.has_next_file_number_) {
        next_file = edit.next_file_number_;
        have_next_file = true;
      }

      if (edit.has_last_sequence_) {
        last_sequence = edit.last_sequence_;
        have_last_sequence = true;
      }
    }
  }
  delete file;
  file = nullptr;

  if (s.ok()) {
    if (!have_next_file) {
      s = Status::Corruption("no meta-nextfile entry in descriptor");
    } else if (!have_log_number) {
      s = Status::Corruption("no meta-lognumber entry in descriptor");
    } else if (!have_last_sequence) {
      s = Status::Corruption("no last-sequence-number entry in descriptor");
    }

    if (!have_prev_log_number) {
      prev_log_number = 0;
    }

    MarkFileNumberUsed(prev_log_number);
    MarkFileNumberUsed(log_number);
  }

  if (s.ok()) {
    Version* v = new Version(this);
    // XXX:生成新的版本V
    builder.SaveTo(v);
    // Install recovered version
    Finalize(v);
    // XXX:添加到version_set中
    AppendVersion(v);
    manifest_file_number_ = next_file;
    next_file_number_ = next_file + 1;
    last_sequence_ = last_sequence;
    log_number_ = log_number;
    prev_log_number_ = prev_log_number;

    // See if we can reuse the existing MANIFEST file.
    if (ReuseManifest(dscname, current)) {
      // No need to save new manifest
    } else {
      *save_manifest = true;
    }
  } else {
    std::string error = s.ToString();
    Log(options_->info_log, "Error recovering version set with %d records: %s",
        read_records, error.c_str());
  }
  return s;
}
```
## log recovery
在`manifest`恢复之后就会使用`log`来恢复数据库。在前面给出的`DBImpl::recover`代码中我们可以发现，日志文件实际上是通过检查数据库目录当中文件的后缀`.log`找到的。恢复时，按照日志文件号(日志文件名字里的序号)的顺序来进行恢复。恢复过程中将日志的写操作记录到临时的`memtable`中，如果`memtable`过大则直接用`memtable`来进行`minor compaction`. 恢复代码如下:



```cpp
Status DBImpl::RecoverLogFile(uint64_t log_number, bool last_log,
                              bool* save_manifest, VersionEdit* edit,
                              SequenceNumber* max_sequence) {
  struct LogReporter : public log::Reader::Reporter {
    Env* env;
    Logger* info_log;
    const char* fname;
    Status* status;  // null if options_.paranoid_checks==false
    void Corruption(size_t bytes, const Status& s) override {
      Log(info_log, "%s%s: dropping %d bytes; %s",
          (this->status == nullptr ? "(ignoring error) " : ""), fname,
          static_cast<int>(bytes), s.ToString().c_str());
      if (this->status != nullptr && this->status->ok()) *this->status = s;
    }
  };

  mutex_.AssertHeld();

  // Open the log file
  std::string fname = LogFileName(dbname_, log_number);
  SequentialFile* file;
  Status status = env_->NewSequentialFile(fname, &file);
  if (!status.ok()) {
    MaybeIgnoreError(&status);
    return status;
  }

  // Create the log reader.
  LogReporter reporter;
  reporter.env = env_;
  reporter.info_log = options_.info_log;
  reporter.fname = fname.c_str();
  reporter.status = (options_.paranoid_checks ? &status : nullptr);
  // We intentionally make log::Reader do checksumming even if
  // paranoid_checks==false so that corruptions cause entire commits
  // to be skipped instead of propagating bad information (like overly
  // large sequence numbers).
  log::Reader reader(file, &reporter, true /*checksum*/, 0 /*initial_offset*/);
  Log(options_.info_log, "Recovering log #%llu",
      (unsigned long long)log_number);

  // Read all the records and add to a memtable
  std::string scratch;
  Slice record;
  WriteBatch batch;
  int compactions = 0;
  // 临时memtable
  MemTable* mem = nullptr;
  // 一条一条读取日志，按照日志内容直接写入临时memtable中
  while (reader.ReadRecord(&record, &scratch) && status.ok()) {
    if (record.size() < 12) {
      reporter.Corruption(record.size(),
                          Status::Corruption("log record too small"));
      continue;
    }
    WriteBatchInternal::SetContents(&batch, record);

    if (mem == nullptr) {
      mem = new MemTable(internal_comparator_);
      mem->Ref();
    }
    // XXX:将读取的writebatch写入memtable
    status = WriteBatchInternal::InsertInto(&batch, mem);
    MaybeIgnoreError(&status);
    if (!status.ok()) {
      break;
    }
    // 一条batch有许多条写操作，序号需要向后移count
    const SequenceNumber last_seq = WriteBatchInternal::Sequence(&batch) +
                                    WriteBatchInternal::Count(&batch) - 1;
    if (last_seq > *max_sequence) {
      *max_sequence = last_seq;
    }
    // 临时memtable过大，跳过变为imm的步骤，直接进行majorcompaction
    // 持久化为sstable
    if (mem->ApproximateMemoryUsage() > options_.write_buffer_size) {
      compactions++;
      *save_manifest = true;
      status = WriteLevel0Table(mem, edit, nullptr);
      mem->Unref();
      mem = nullptr;
      if (!status.ok()) {
        // Reflect errors immediately so that conditions like full
        // file-systems cause the DB::Open() to fail.
        break;
      }
    }
  }

  delete file;

  // See if we should keep reusing the last log file.
  // 如果reuse_logs为true且恢复最后一个compaction
  if (status.ok() && options_.reuse_logs && last_log && compactions == 0) {
    assert(logfile_ == nullptr);
    assert(log_ == nullptr);
    assert(mem_ == nullptr);
    uint64_t lfile_size;
    if (env_->GetFileSize(fname, &lfile_size).ok() &&
        env_->NewAppendableFile(fname, &logfile_).ok()) {
      Log(options_.info_log, "Reusing old log %s \n", fname.c_str());
      log_ = new log::Writer(logfile_, lfile_size);
      logfile_number_ = log_number;
      if (mem != nullptr) {
        mem_ = mem;
        mem = nullptr;
      } else {
        // mem can be nullptr if lognum exists but was empty.
        mem_ = new MemTable(internal_comparator_);
        mem_->Ref();
      }
    }
  }

  if (mem != nullptr) {
    // mem did not get reused; compact it.
    if (status.ok()) {
      *save_manifest = true;
      status = WriteLevel0Table(mem, edit, nullptr);
    }
    mem->Unref();
  }

  return status;
}
```


TODO:sizecompaction 最开始选谁？
