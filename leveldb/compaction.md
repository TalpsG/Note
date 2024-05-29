# compaction
`LevelDB` 的 `compaction` 机制是它确保读写性能、优化存储空间利用、并减少磁盘读写放大的一种关键过程。`Compaction`主要涉及将多个 `SSTable`文件合并为一个文件，以及清理过期的键值对。

在`leveldb`中通过`MaybeScheduleCompaction`在可能会`compaction`的地方进行可能的压缩工作，比如`get`之后(`seek_compaction`后面会讲)。在其他确定会压缩的地方会执行其他的函数比如`CompactMemTable`来进行压缩。
```cpp
void DBImpl::BackgroundCompaction() {
  mutex_.AssertHeld();

  // minor compaction
  if (imm_ != nullptr) {
    CompactMemTable();
    return;
  }
  // major compaction codes 
  // ...
}

```

`MaybeScheduleCompaction`会开一个后台线程来进行`compaction`的操作。

`compaction`分为`minor`和`major`，`minor`就是将`imm`转化为磁盘`sstable`的过程。而`major`则是整理磁盘上的`sstable`的过程。


下面先看看`minor compaction`
# minor compaction
下面就是执行`minor compaction`的两个函数，`CompactMemTable`调用`WriteLevel0Table`将`imm`转化为`sstable`，用`edit`记录了`sstable`的变化，并且生成新的版本。最后会执行`RemoveObsoleteFiles`清除现在所有活跃版本都没有使用到的文件，这个函数`major compaction`之后也会调用，因此放在`major`时讲。
```cpp
void DBImpl::CompactMemTable() {
  mutex_.AssertHeld();
  assert(imm_ != nullptr);
  // Save the contents of the memtable as a new Table
  VersionEdit edit;
  // 记录当前版本
  Version* base = versions_->current();
  base->Ref();
  // 执行minorcompaction
  Status s = WriteLevel0Table(imm_, &edit, base);
  base->Unref();

  if (s.ok() && shutting_down_.load(std::memory_order_acquire)) {
    s = Status::IOError("Deleting DB during memtable compaction");
  }

  // Replace immutable memtable with the generated Table
  if (s.ok()) {
    edit.SetPrevLogNumber(0);
    edit.SetLogNumber(logfile_number_);  // Earlier logs no longer needed
    s = versions_->LogAndApply(&edit, &mutex_);
  }

  if (s.ok()) {
    // Commit to the new state
    imm_->Unref();
    imm_ = nullptr;
    has_imm_.store(false, std::memory_order_release);
    RemoveObsoleteFiles();
  } else {
    RecordBackgroundError(s);
  }
}
Status DBImpl::WriteLevel0Table(MemTable* mem, VersionEdit* edit,
                                Version* base) {
  mutex_.AssertHeld();
  const uint64_t start_micros = env_->NowMicros();
  FileMetaData meta;
  meta.number = versions_->NewFileNumber();
  pending_outputs_.insert(meta.number);
  Iterator* iter = mem->NewIterator();
  Log(options_.info_log, "Level-0 table #%llu: started",
      (unsigned long long)meta.number);

  Status s;
  {
    mutex_.Unlock();
    s = BuildTable(dbname_, env_, options_, table_cache_, iter, &meta);
    mutex_.Lock();
  }

  Log(options_.info_log, "Level-0 table #%llu: %lld bytes %s",
      (unsigned long long)meta.number, (unsigned long long)meta.file_size,
      s.ToString().c_str());
  delete iter;
  pending_outputs_.erase(meta.number);

  // Note that if file_size is zero, the file has been deleted and
  // should not be added to the manifest.
  int level = 0;
  if (s.ok() && meta.file_size > 0) {
    const Slice min_user_key = meta.smallest.user_key();
    const Slice max_user_key = meta.largest.user_key();
    if (base != nullptr) {
      // XXX:
      // 此处是一个优化
      // 将sstable向下推到不重叠的最高层
      level = base->PickLevelForMemTableOutput(min_user_key, max_user_key);
    }
    // XXX:添加到edit中
    edit->AddFile(level, meta.number, meta.file_size, meta.smallest,
                  meta.largest);
  }

  CompactionStats stats;
  stats.micros = env_->NowMicros() - start_micros;
  stats.bytes_written = meta.file_size;
  stats_[level].Add(stats);
  return s;
}
```
请注意这里`leveldb`在`sstable`的层数选择上做了优化。
本来新的`sstable`要放在`level0`的，考虑到后续`major compaction`都是对重叠的`sstable`进行合并，因此如果新的`sstable`与`level0-n`都不重叠，就可以将其放到`leveln`当中，这样对也避免了后续`major compaction`对该`sstable`的操作。下面这个函数就是挑选`level`的具体实现：最开始`level`为0，与下一层不重叠就自增。此处也判断了`level+2`的重叠情况，如果与`level+2`的重叠过多，会导致在`level+1`上的`major compaction`的工作量太大。
```cpp
int Version::PickLevelForMemTableOutput(const Slice& smallest_user_key,
                                        const Slice& largest_user_key) {
  int level = 0;
  if (!OverlapInLevel(0, &smallest_user_key, &largest_user_key)) {
    // Push to next level if there is no overlap in next level,
    // and the #bytes overlapping in the level after that are limited.
    InternalKey start(smallest_user_key, kMaxSequenceNumber, kValueTypeForSeek);
    InternalKey limit(largest_user_key, 0, static_cast<ValueType>(0));
    std::vector<FileMetaData*> overlaps;
    while (level < config::kMaxMemCompactLevel) {
      if (OverlapInLevel(level + 1, &smallest_user_key, &largest_user_key)) {
        break;
      }
      if (level + 2 < config::kNumLevels) {
        // Check that file does not overlap too many grandparent bytes.
        // XXX:不能与level+2重叠太多
        GetOverlappingInputs(level + 2, &start, &limit, &overlaps);
        const int64_t sum = TotalFileSize(overlaps);
        if (sum > MaxGrandParentOverlapBytes(vset_->options_)) {
          break;
        }
      }
      level++;
    }
  }
  return level;
}
```
下面我们来看一下`imm`是如何转变为`sstable`的。
由于`memtable`和`imm`的结构都是跳表，因此很容易实现其迭代器，`BuildTable`就是用其迭代器获取的`kv`来构造`sstable`的。
`sstable`的格式第一篇文章中讲过，不再赘述了。
```cpp
Status BuildTable(const std::string& dbname, Env* env, const Options& options,
                  TableCache* table_cache, Iterator* iter, FileMetaData* meta) {
  Status s;
  meta->file_size = 0;
  iter->SeekToFirst();

  std::string fname = TableFileName(dbname, meta->number);
  if (iter->Valid()) {
    WritableFile* file;
    s = env->NewWritableFile(fname, &file);
    if (!s.ok()) {
      return s;
    }

    TableBuilder* builder = new TableBuilder(options, file);
    // memtable中的数据本身就是有序的，因此第一个就是最小的
    meta->smallest.DecodeFrom(iter->key());
    Slice key;
    for (; iter->Valid(); iter->Next()) {
      key = iter->key();
      builder->Add(key, iter->value());
    }
    // 最后一个就是最大的
    if (!key.empty()) {
      meta->largest.DecodeFrom(key);
    }

    // Finish and check for builder errors
    s = builder->Finish();
    if (s.ok()) {
      meta->file_size = builder->FileSize();
      assert(meta->file_size > 0);
    }
    delete builder;

    // Finish and check for file errors
    if (s.ok()) {
      s = file->Sync();
    }
    if (s.ok()) {
      s = file->Close();
    }
    delete file;
    file = nullptr;

    if (s.ok()) {
      // Verify that the table is usable
      Iterator* it = table_cache->NewIterator(ReadOptions(), meta->number,
                                              meta->file_size);
      s = it->status();
      delete it;
    }
  }

  // Check for input iterator errors
  if (!iter->status().ok()) {
    s = iter->status();
  }

  if (s.ok() && meta->file_size > 0) {
    // Keep it
  } else {
    env->RemoveFile(fname);
  }
  return s;
}
```
`minorcompaction`的过程还是比较清楚明了的，下面我们看看`majorcompaction`
# major compaction
`majorcompaction`实际上是整理磁盘上的`sstable`的操作。
`major compaction`也分为两种。
一种是`size_compaction`，当某一个`level`的`sstable`总大小过大时，就要对其进行压缩并下移。
另外一种是`seek_compaction`，由于读磁盘的操作很费时间，因此对每一个`sstable`都会维护一个`allowed_seek`计数器，这个计数器的是通过计算得到的。在`get`时如果查找的第一个`sstable` miss了，就会对该`sstable`的`allowed_seek`减一(并不是立刻去做的，而是在`updatestats`中进行的)。当某一个`sstable`的`allowed_seek`为0时，就要对其进行`seek_compaction`，降低读放大。
```cpp
// We arrange to automatically compact this file after
// a certain number of seeks.  Let's assume:
//   (1) One seek costs 10ms
//   (2) Writing or reading 1MB costs 10ms (100MB/s)
//   (3) A compaction of 1MB does 25MB of IO:
//         1MB read from this level
//         10-12MB read from next level (boundaries may be misaligned)
//         10-12MB written to next level
// This implies that 25 seeks cost the same as the compaction
// of 1MB of data.  I.e., one seek costs approximately the
// same as the compaction of 40KB of data.  We are a little
// conservative and allow approximately one seek for every 16KB
// of data before triggering a compaction.

f->allowed_seeks = static_cast<int>((f->file_size / 16384U));
if (f->allowed_seeks < 100) f->allowed_seeks = 100;
```

`leveldb`后台会自动`majorcompaction`，用户也可以手动指定`compaction`的范围，然后执行`major compaction`，下面是后台`compaction`的函数: `major`部分调用`PickCompaction`选出了需要压缩的`sstable`集合，之后进行归并的操作。
```cpp
void DBImpl::BackgroundCompaction() {
  mutex_.AssertHeld();

  // minor compaction
  if (imm_ != nullptr) {
    CompactMemTable();
    return;
  }

  Compaction* c;
  bool is_manual = (manual_compaction_ != nullptr);
  InternalKey manual_end;
  // 可能是手动compaction
  if (is_manual) {
    ManualCompaction* m = manual_compaction_;
    c = versions_->CompactRange(m->level, m->begin, m->end);
    m->done = (c == nullptr);
    if (c != nullptr) {
      manual_end = c->input(0, c->num_input_files(0) - 1)->largest;
    }
    Log(options_.info_log,
        "Manual compaction at level-%d from %s .. %s; will stop at %s\n",
        m->level, (m->begin ? m->begin->DebugString().c_str() : "(begin)"),
        (m->end ? m->end->DebugString().c_str() : "(end)"),
        (m->done ? "(end)" : manual_end.DebugString().c_str()));
  } else {
    // XXX:
    // 按size_compaction或seek_compaction来选择需要合并的sstable
    c = versions_->PickCompaction();
  }

  Status status;
  if (c == nullptr) {
    // Nothing to do
  } else if (!is_manual && c->IsTrivialMove()) {
    // XXX:只需要移动不需要归并排序
    // Move file to next level
    assert(c->num_input_files(0) == 1);
    FileMetaData* f = c->input(0, 0);
    c->edit()->RemoveFile(c->level(), f->number);
    c->edit()->AddFile(c->level() + 1, f->number, f->file_size, f->smallest,
                       f->largest);
    status = versions_->LogAndApply(c->edit(), &mutex_);
    if (!status.ok()) {
      RecordBackgroundError(status);
    }
    VersionSet::LevelSummaryStorage tmp;
    Log(options_.info_log, "Moved #%lld to level-%d %lld bytes %s: %s\n",
        static_cast<unsigned long long>(f->number), c->level() + 1,
        static_cast<unsigned long long>(f->file_size),
        status.ToString().c_str(), versions_->LevelSummary(&tmp));
  } else {
    CompactionState* compact = new CompactionState(c);
    // XXX:需要归并排序
    status = DoCompactionWork(compact);
    if (!status.ok()) {
      RecordBackgroundError(status);
    }
    CleanupCompaction(compact);
    c->ReleaseInputs();
    // XXX:
    // 删除无效的文件
    RemoveObsoleteFiles();
  }
  delete c;

  if (status.ok()) {
    // Done
  } else if (shutting_down_.load(std::memory_order_acquire)) {
    // Ignore compaction errors found during shutting down
  } else {
    Log(options_.info_log, "Compaction error: %s", status.ToString().c_str());
  }

  if (is_manual) {
    ManualCompaction* m = manual_compaction_;
    if (!status.ok()) {
      m->done = true;
    }
    if (!m->done) {
      // We only compacted part of the requested range.  Update *m
      // to the range that is left to be compacted.
      m->tmp_storage = manual_end;
      m->begin = &m->tmp_storage;
    }
    manual_compaction_ = nullptr;
  }
}
```
`PickCompaction`的代码中我们可以看到`size_compaction`的优先级高于`seek_compaction`。在函数刚开始时先根据`major compaction`类型选取`input0`。 假设合并是在`level n`进行的，`input0`就是`level n`中需要压缩合并的`sstable`集合，`input1`就是`level n+1`的。然后执行`SetupOtherInputs`对`input`集合进行补充(选择和`input0`重叠的下一层`level`的`sstable`)。
先看看`size_compaction`。`size_compaction`的条件是`compaction_score_>=1`，条件达成，则在`compaction_level_`挑选合适的`sstable`与下一级别的合并。`compaction_score_`是在每次`compaction`后用`versionedit`生成新版本时的`finalize`计算的，与之一起的还有一个`compaction_level_`。这两个属性表示的是最高得分及其`level`，对于`level0`来说，得分与`level0sstable`数量有关。对于其他`level`而言，得分与该`level`的总大小有关。具体信息可以自行查看`finalize`函数。
`size_compaction`挑选的`sstable`是通过`compact_pointer_`来挑选的，这是一个`vector`记录了每一层中压缩到了哪个`key`。`size_compaction`需要挑选出最大`internalkey` 大于`compact_pointer_`的第一个`sstable`添加到`input0`。

`seek_compaction`则是`file_to_compact_`不为空就要将该`table`添加到`input0`中。

找到`input0`的`sstable`后，对`level0`进行了特殊处理，因为`level0`的`sstable`是重叠的，因此需要扩充`input0`。扩充的方式很暴力：每选出一个与`input0`重叠的`sstable`加入`input0`后会维护`input0`的范围。然后继续从`levle0`中查找重叠的`sstable`，直至没有重叠的为止。

再然后执行了`SetupOtherInputs`来进一步补充待压缩合并的`sstable`集合。
```cpp
Compaction* VersionSet::PickCompaction() {
  Compaction* c;
  int level;

  // We prefer compactions triggered by too much data in a level over
  // the compactions triggered by seeks.
  const bool size_compaction = (current_->compaction_score_ >= 1);
  const bool seek_compaction = (current_->file_to_compact_ != nullptr);
  if (size_compaction) {
    level = current_->compaction_level_;
    assert(level >= 0);
    assert(level + 1 < config::kNumLevels);
    c = new Compaction(options_, level);

    // Pick the first file that comes after compact_pointer_[level]
    for (size_t i = 0; i < current_->files_[level].size(); i++) {
      FileMetaData* f = current_->files_[level][i];
      // XXX:挑选compact pointer之后的sstable放入input0
      if (compact_pointer_[level].empty() ||
          icmp_.Compare(f->largest.Encode(), compact_pointer_[level]) > 0) {
        c->inputs_[0].push_back(f);
        break;
      }
    }
    if (c->inputs_[0].empty()) {
      // Wrap-around to the beginning of the key space
      c->inputs_[0].push_back(current_->files_[level][0]);
    }
  } else if (seek_compaction) {
    level = current_->file_to_compact_level_;
    c = new Compaction(options_, level);
    c->inputs_[0].push_back(current_->file_to_compact_);
  } else {
    return nullptr;
  }

  c->input_version_ = current_;
  c->input_version_->Ref();

  // Files in level 0 may overlap each other, so pick up all overlapping ones
  if (level == 0) {
    InternalKey smallest, largest;
    GetRange(c->inputs_[0], &smallest, &largest);
    // Note that the next call will discard the file we placed in
    // c->inputs_[0] earlier and replace it with an overlapping set
    // which will include the picked file.
    current_->GetOverlappingInputs(0, &smallest, &largest, &c->inputs_[0]);
    assert(!c->inputs_[0].empty());
  }

  SetupOtherInputs(c);

  return c;
}
```
`SetupOtherInputs`的代码如下，其中的操作繁杂，这里只说一下`AddBoundaryInputs`的行为，其他的请看下面代码内的注释。
`AddBoundaryInputs`的行为是将同一个`level`中边界重叠的`sstable`也添加到对应的`input[0/1]`中去。
边界重叠的就是`input[0/1]`中的`sstable`的最大`internalkey`的`userkey`与该`level`中的其他的`sstable`的最小`internalkey`的`userkey`相同，即为边界重叠。边界重叠仅仅单向去判断，也就是`input[0/1]`中的`sstable`的最大和其他该`level`中的`sstable`的最小进行比较。
为什么要这样呢？这里设想一种情况:
`level1`当中有`sstable a,b`两个文件，`a`的`userkey`范围是`1-100`，`b`的范围是`100-200`。因为`sstable`中的`kv`是按照`internalkey`排序的，因此`a`中的`100`数据是要比`b`的更新的。此时对`100`的查询是正确的。但是如果此时要合并压缩`a`，`b`是和`a`边界重叠的，不添加`b`会导致`a`和下层压缩过后，`level1`中`userkey`为`100`的数据不是最新的，查询会出错。


```cpp
void VersionSet::SetupOtherInputs(Compaction* c) {
  const int level = c->level();
  InternalKey smallest, largest;

  /*
   * XXX:
   * level的sstable之前就拿到了部分
   * 调用AddBoundaryInputs获取边界重叠的sstable
   * 获取input中的range，此时input中有level的sstable
   */
  AddBoundaryInputs(icmp_, current_->files_[level], &c->inputs_[0]);
  GetRange(c->inputs_[0], &smallest, &largest);

  /*
   * XXX:
   * 获取level+1
   * 通过GetOverlappingInputs获取level+1 的sstable
   * 通过AddBoundaryInputs获取level+1中边界重叠的sstable
   * getrange2获取inputs中的范围
   *
   */
  current_->GetOverlappingInputs(level + 1, &smallest, &largest,
                                 &c->inputs_[1]);
  AddBoundaryInputs(icmp_, current_->files_[level + 1], &c->inputs_[1]);

  // Get entire range covered by compaction
  InternalKey all_start, all_limit;
  GetRange2(c->inputs_[0], c->inputs_[1], &all_start, &all_limit);

  // See if we can grow the number of inputs in "level" without
  // changing the number of "level+1" files we pick up.

  if (!c->inputs_[1].empty()) {
    /*
     * XXX:
     * level+1 的sstable非空
     * 则上面getrange2得到的range与第一次得到的可能不同
     * 需要按照新的range获取sstable到expanded0
     */
    std::vector<FileMetaData*> expanded0;
    current_->GetOverlappingInputs(level, &all_start, &all_limit, &expanded0);
    AddBoundaryInputs(icmp_, current_->files_[level], &expanded0);
    const int64_t inputs0_size = TotalFileSize(c->inputs_[0]);
    const int64_t inputs1_size = TotalFileSize(c->inputs_[1]);
    const int64_t expanded0_size = TotalFileSize(expanded0);
    /*
     * XXX:
     * expanded0就是getrange2的范围的level中的sstable集合
     * 如果比getrange中的多，证明扩容了
     */
    if (expanded0.size() > c->inputs_[0].size() &&
        inputs1_size + expanded0_size <
            ExpandedCompactionByteSizeLimit(options_)) {
      /*
       *  XXX:
       *  如果compaction的内容太大则直接跳过
       */
      InternalKey new_start, new_limit;
      /*
       * XXX:
       * 获取expanded0的range然后
       * 重新添加level+1层的sstable到expanded1
       *
       */
      GetRange(expanded0, &new_start, &new_limit);
      std::vector<FileMetaData*> expanded1;
      current_->GetOverlappingInputs(level + 1, &new_start, &new_limit,
                                     &expanded1);
      AddBoundaryInputs(icmp_, current_->files_[level + 1], &expanded1);
      /*
       * XXX:
       * 如果expanded1没变，则将
       * input0  expanded0
       * input1 expanded1
       * 变了就终止了，还要原来的input0/1
       * expanded0和1都不要
       * 否则上面加完下面加
       * 没完没了了
       */
      if (expanded1.size() == c->inputs_[1].size()) {
        Log(options_->info_log,
            "Expanding@%d %d+%d (%ld+%ld bytes) to %d+%d (%ld+%ld bytes)\n",
            level, int(c->inputs_[0].size()), int(c->inputs_[1].size()),
            long(inputs0_size), long(inputs1_size), int(expanded0.size()),
            int(expanded1.size()), long(expanded0_size), long(inputs1_size));
        smallest = new_start;
        largest = new_limit;
        c->inputs_[0] = expanded0;
        c->inputs_[1] = expanded1;
        GetRange2(c->inputs_[0], c->inputs_[1], &all_start, &all_limit);
      }
    }
  }

  // Compute the set of grandparent files that overlap this compaction
  // (parent == level+1; grandparent == level+2)
  if (level + 2 < config::kNumLevels) {
    current_->GetOverlappingInputs(level + 2, &all_start, &all_limit,
                                   &c->grandparents_);
  }

  // Update the place where we will do the next compaction for this level.
  // We update this immediately instead of waiting for the VersionEdit
  // to be applied so that if the compaction fails, we will try a different
  // key range next time.
  compact_pointer_[level] = largest.Encode().ToString();
  c->edit_.SetCompactPointer(level, largest);
}
```
以上的操作都执行完成后，就该进行归并输出新的`sstable`了，最后来看看`DoCompactionWork`的实现:
最开始先记录了`smallest_snapshot`，记录过`snapshot`之后利用`mergingiter`来遍历`sstable`，将其归并输出一个或几个新的`sstable`当中，最后生成新的版本。
`smallest_snapshot`其实是一个`sequence_number`，`leveldb`用`snapshot`来保证数据的一致性。
`smallest_snapshot`代表着用户可能会查询`sequence_number = smallest_snapshot`时的数据，因此对于每一个`userkey`，如果记录的`sequence_number`大于`smallest_snapshot`那么都要保存，如果`sequence_number`小于`smallest_snapshot`则只需要保存一条最大的`sequence_number`小于`smallest_snapshot`的记录即可。
由于`sstable`的`internalkey`是按照`seq`降序排列的，因此对于相同的`userkey`的操作，大于`smallest_snapshot`保存，小于`smallest_snapshot`只保存第一条即可，后面的`seq`越来越小。

```cpp
Status DBImpl::DoCompactionWork(CompactionState* compact) {

  if (snapshots_.empty()) {
    compact->smallest_snapshot = versions_->LastSequence();
  } else {
    compact->smallest_snapshot = snapshots_.oldest()->sequence_number();
  }
  // ......

  // XXX:生成mergingiter
  Iterator* input = versions_->MakeInputIterator(compact->compaction);

  // Release mutex while we're actually doing the compaction work
  mutex_.Unlock();

  input->SeekToFirst();
  Status status;
  ParsedInternalKey ikey;
  std::string current_user_key;
  bool has_current_user_key = false;
  SequenceNumber last_sequence_for_key = kMaxSequenceNumber;
  while (input->Valid() && !shutting_down_.load(std::memory_order_acquire)) {
    // Prioritize immutable compaction work
    // XXX:imm优先compaction
    if (has_imm_.load(std::memory_order_relaxed)) {
      const uint64_t imm_start = env_->NowMicros();
      mutex_.Lock();
      if (imm_ != nullptr) {
        CompactMemTable();
        // Wake up MakeRoomForWrite() if necessary.
        background_work_finished_signal_.SignalAll();
      }
      mutex_.Unlock();
      imm_micros += (env_->NowMicros() - imm_start);
    }

    Slice key = input->key();
    // XXX:
    // level+1与level+2的重叠部分过多会导致level+1向下的compaction很慢
    // 因此过多则停止
    if (compact->compaction->ShouldStopBefore(key) &&
        compact->builder != nullptr) {
      status = FinishCompactionOutputFile(compact, input);
      if (!status.ok()) {
        break;
      }
    }

    // Handle key/value, add to state, etc.
    bool drop = false;
    // XXX:
    // 取出iter中的internal_key到ikey
    if (!ParseInternalKey(key, &ikey)) {
      // Do not hide error keys
      current_user_key.clear();
      has_current_user_key = false;
      last_sequence_for_key = kMaxSequenceNumber;
    } else {
      if (!has_current_user_key ||
          user_comparator()->Compare(ikey.user_key, Slice(current_user_key)) !=
              0) {
        // First occurrence of this user key
        // XXX:
        // userkey第一次出现时，不能删除
        current_user_key.assign(ikey.user_key.data(), ikey.user_key.size());
        has_current_user_key = true;
        // XXX: 记录上次当前userkey出现的seq
        last_sequence_for_key = kMaxSequenceNumber;
      }

      // XXX:
      // userkey第一次出现时last_sequence_for_key  = kMaxSequenceNumber,一定会保存
      // 后面的last_sequence_for_key则记录了current_user_key上一条记录的sequence
      // 由于是降序排列，对于相同的userkey只需要一条小于smallest_snapshot的记录即可，
      // 则后面该userkey的操作都要drop为true
      if (last_sequence_for_key <= compact->smallest_snapshot) {
        // Hidden by an newer entry for same user key
        drop = true;  // (A)
      } else if (ikey.type == kTypeDeletion &&
                 ikey.sequence <= compact->smallest_snapshot &&
                 compact->compaction->IsBaseLevelForKey(ikey.user_key)) {
        // XXX:
        // 当userkey第一次出现时，如果seq小于smallest_snapshot
        // 且是删除操作，且下面level中没有该userkey的记录
        // 则删除该条记录。
        //
        // For this user key:
        // (1) there is no data in higher levels
        // (2) data in lower levels will have larger sequence numbers
        // (3) data in layers that are being compacted here and have
        //     smaller sequence numbers will be dropped in the next
        //     few iterations of this loop (by rule (A) above).
        // Therefore this deletion marker is obsolete and can be dropped.
        drop = true;
      }

      last_sequence_for_key = ikey.sequence;
    }
#if 0
    Log(options_.info_log,
        "  Compact: %s, seq %d, type: %d %d, drop: %d, is_base: %d, "
        "%d smallest_snapshot: %d",
        ikey.user_key.ToString().c_str(),
        (int)ikey.sequence, ikey.type, kTypeValue, drop,
        compact->compaction->IsBaseLevelForKey(ikey.user_key),
        (int)last_sequence_for_key, (int)compact->smallest_snapshot);
#endif

    if (!drop) {
      // Open output file if necessary
      // XXX:没开文件就开一个放新的level+1的sstable
      if (compact->builder == nullptr) {
        status = OpenCompactionOutputFile(compact);
        if (!status.ok()) {
          break;
        }
      }
      if (compact->builder->NumEntries() == 0) {
        compact->current_output()->smallest.DecodeFrom(key);
      }
      compact->current_output()->largest.DecodeFrom(key);
      compact->builder->Add(key, input->value());

      // Close output file if it is big enough
      // XXX:新的sstable太大就关闭该文件
      // 下次循环会重新创建一个
      if (compact->builder->FileSize() >=
          compact->compaction->MaxOutputFileSize()) {
        status = FinishCompactionOutputFile(compact, input);
        if (!status.ok()) {
          break;
        }
      }
    }

    input->Next();
  }

  if (status.ok() && shutting_down_.load(std::memory_order_acquire)) {
    status = Status::IOError("Deleting DB during compaction");
  }
  if (status.ok() && compact->builder != nullptr) {
    status = FinishCompactionOutputFile(compact, input);
  }
  if (status.ok()) {
    status = input->status();
  }
  delete input;
  input = nullptr;

  CompactionStats stats;
  stats.micros = env_->NowMicros() - start_micros - imm_micros;
  for (int which = 0; which < 2; which++) {
    for (int i = 0; i < compact->compaction->num_input_files(which); i++) {
      stats.bytes_read += compact->compaction->input(which, i)->file_size;
    }
  }
  for (size_t i = 0; i < compact->outputs.size(); i++) {
    stats.bytes_written += compact->outputs[i].file_size;
  }

  mutex_.Lock();
  // XXX:记录数据统计，写入大小读入大小等
  stats_[compact->compaction->level() + 1].Add(stats);

  // XXX:将compaction应用到新版本中
  if (status.ok()) {
    status = InstallCompactionResults(compact);
  }
  if (!status.ok()) {
    RecordBackgroundError(status);
  }
  VersionSet::LevelSummaryStorage tmp;
  Log(options_.info_log, "compacted to: %s", versions_->LevelSummary(&tmp));
  return status;
}
```

此后`compaction`工作就已经完成了，但是其实还有些后续工作。`compaction`并不会删除归并压缩过的`sstable`，因为可能会有其他的版本正在引用参与合并的`sstable`。但是永远不删也不行，`leveldb`中在`compaction`结束后会执行`RemoveObsoleteFiles`来删除无用的文件。

`RemoveObsoleteFiles`通过获取数据库目录下的所有文件，遍历其类型，然后判断是否还需要保留，将不需要保留的删除。
比如`sstable`类型的文件，如果不在`versions`的`live`集合中，就证明没有`version`还在引用这个`table`，也就不需要保留这个文件了。
```cpp
void DBImpl::RemoveObsoleteFiles() {
  // ...

  // Make a set of all of the live files
  std::set<uint64_t> live = pending_outputs_;
  // XXX:versions_是所有版本，将现在活跃的sstable添加进去
  versions_->AddLiveFiles(&live);

  std::vector<std::string> filenames;
  env_->GetChildren(dbname_, &filenames);  // Ignoring errors on purpose
  uint64_t number;
  FileType type;
  std::vector<std::string> files_to_delete;
  for (std::string& filename : filenames) {
    if (ParseFileName(filename, &number, &type)) {
      bool keep = true;
      switch (type) {
        case kLogFile:
          // XXX:日志文件小于新版本号或且不等于上一个日志文件号时需要删除日志文件
          keep = ((number >= versions_->LogNumber()) ||
                  (number == versions_->PrevLogNumber()));
          break;
        case ....
        case kTableFile:
          // XXX:live中没有就删除
          keep = (live.find(number) != live.end());
          break;
        case ....
      }

      if (!keep) {
        files_to_delete.push_back(std::move(filename));
        if (type == kTableFile) {
          table_cache_->Evict(number);
        }
        Log(options_.info_log, "Delete type=%d #%lld\n", static_cast<int>(type),
            static_cast<unsigned long long>(number));
      }
    }
  }

  // While deleting all files unblock other threads. All files being deleted
  // have unique names which will not collide with newly created files and
  // are therefore safe to delete while allowing other threads to proceed.
  mutex_.Unlock();
  // 删除文件
  for (const std::string& filename : files_to_delete) {
    env_->RemoveFile(dbname_ + "/" + filename);
  }
  mutex_.Lock();
}
```


# 总结
这是本人第一次读这种算是工业级？的代码，读代码花了大概一个多礼拜，总结花了三五天左右，肯定有所遗漏。如果有哪里不明白或者我写的有问题，欢迎提问/批评指正！






