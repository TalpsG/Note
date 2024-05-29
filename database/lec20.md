# 恢复
检查点带来的挑战
1. 设置检查点时需要停机暂停所有事务。
2. 恢复时扫描日志很费时间
3. 设置检查点的频率不好确定

# aries算法
aries是优化过后的wal,同样使用检查点，专门对数据恢复过程进行了优化。

在恢复时分为三个步骤
1. 分析，确定宕机时数据库的活跃事务和脏页
2. 重做
3. 撤销

重建ATT和DPT来保存活跃事务和脏页。


需要对现有的日志进行扩展，添加了`Log Sequence Number LSN`字段，表示日志号。

| name | location | defination |
| --------------- | --------------- | --------------- |
| flushedLSN | Mem | 上次写入磁盘的lsn |
| pagelsn | page | 修改该页面的最新的lsn |
| reclsn | page | 最老的没有被写入磁盘的修改页面的lsn |
| lastlsn | txn | 事务最新的日志lsn | 
| masterrecord | disk | 指向最新的检查点 |

每个页面都有pagelsn,用来记录最新的日志号
在刷盘的时候，flushedlsn必须大于pagelsn。反之则表示当前页面有其他更新的事务修改过了。

在事务修改page时一并修改pagelsn
在dbms将日志写入磁盘时修改flushedlsn

当日志提交时 dbms写入commit到日志，并且开始将日志刷盘，刷完盘后txn end到日志(不需要立刻刷盘,txn end的目的是为了方便dbms管理日志中的信息)

为了方便事务回滚，对每个日志添加了一个prevlsn字段，用于指示该日志所属事务的上一条日志lsn.

## compensation log record
clr记录了回滚操作的日志。
在日志中添加一个undonext字段指向下一个该回滚的事务的第一个日志lsn

dbms不需要等待clr也刷盘，clr记录到内存就可以视为事务回滚完毕


回滚流程
1. 写abort到日志
2. 反向撤销事务的修改
  - 先写clr,undonext指向要回退的日志
  - 后恢复值

## 模糊检查点
检查点不是一个点了，而是一个时间段。

检查点日志分为begin和end，end处包含了ATT和DPT

masterrecord 记录了最新的检查点的begin位置

分为三个阶段
1. 分析
  - 分析脏页和活跃事务
2. 重做
3. 撤销


### 分析
正向分析日志，将所有日志的事务都添加到att中，如果txnend则从att中删除该事务，如果事务commit,则修改其状态为`COMMIT`，剩下的事务的状态都是`UNDO`

对每条update日志，如果页面没有在DPT中，添加页面到DPT中，并且修改reclsn,记录最老的没提交的修改

## 重做
从最小的DPT中reclsn开始扫描，对每个更新日志除非
1. 页面不在DPT中不作修改
2. 日志lsn 小于reclsn(最早的更新的日志号，此前的日志号的页面更新都刷盘了)

对页面重写后需要修改pagelsn,redo最后要写TXNend表示所有提交过的事务都重做过了。将提交过的事务从ATT删除.


## 撤销
通过lastlsn来追踪事务的日志。
撤销时要写clr日志
