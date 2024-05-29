# memtable 迭代器 
之前讲过了`memtable`的查找和写入的流程，现在我们来看看`memtable`的迭代器的`next`是如何实现的。


我们看下`dbiter`中`next`的代码:
```cpp
void DBIter::Next() {
  assert(valid_);
  if (direction_ == kReverse) {  // Switch directions?
    // 反向迭代器，就不看了主要关注正向
  } else {
    // Store in saved_key_ the current key so we skip it below.
    // 保存当前 key 用于向后遍历的时候跳过相同的key
    SaveKey(ExtractUserKey(iter_->key()), &saved_key_);


    // iter_ is pointing to current key. We can now safely move to the next to
    // avoid checking current key.
    iter_->Next();
    if (!iter_->Valid()) {
      valid_ = false;
      saved_key_.clear();
      return;
    }
  }
  FindNextUserEntry(true, &saved_key_);
}
```

保存当前的`key`用于之后跳过相同的`key，`，此后`next`向后挪一个位置。在`memtable`上由于使用的是跳表，因此很容易实现，只需要顺着链表向后走一个即可。
然后使用`FindNextUserEntry`函数。

```cpp
void DBIter::FindNextUserEntry(bool skipping, std::string* skip) {
  // Loop until we hit an acceptable entry to yield
  assert(iter_->Valid());
  assert(direction_ == kForward);
  do {
    ParsedInternalKey ikey;
    // seq大于则该迭代器的seq则直接跳过
    // iter不会遍历到未来的数据
    if (ParseKey(&ikey) && ikey.sequence <= sequence_) {
      switch (ikey.type) {
        case kTypeDeletion:
          // Arrange to skip all upcoming entries for this key since
          // they are hidden by this deletion.
          SaveKey(ikey.user_key, skip);
          skipping = true;
          break;
        case kTypeValue:
          if (skipping &&
              user_comparator_->Compare(ikey.user_key, *skip) <= 0) {
            // Entry hidden
          } else {
            valid_ = true;
            saved_key_.clear();
            return;
          }
          break;
      }
    }
    iter_->Next();
  } while (iter_->Valid());
  saved_key_.clear();
  valid_ = false;
}
```
在该函数中，`internalkey`的`seq`大于`iter`的`seq`则会被跳过，这很好理解，毕竟不能使用来自未来的数据。之后判断如果是`delete`的`internalkey`则跳过，并更改`saved_key_`用于后面的跳过(因为跳表按`internalkey`排序，因此`key`相同的都在一起)。遇到第一条大于`skip`的即为找到了`next`的位置。在此停下即可。
