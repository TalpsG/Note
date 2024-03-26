# 顺序容器
顺序容器有
`vector`,`deque`,`array`,`list` `forward_list`这五个


`vector`并不检查边界，因为会降低性能。

`deque`是一个双端队列，解决了`vector`无法头插的问题。

`deque`可以快速实现头插的操作，但是在下标索引的操作上速度是不如`vector`的


如果在容器的头或尾有大量的删除插入操作，那么应该使用`deque`，否则用`vector`



