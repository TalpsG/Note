# ext2fs 文件系统的日志

## abstract
这篇论文描述了一个 还在进展中的工作，这个工作为`linux`的`ext2fs`文件系统设计实现了一个事务性元数据日志。本文回顾了崩溃后的文件系统恢复的问题，描述了一种通过给文件系统增加事务性日志的方法，增加了`ext2fs`崩溃恢复的速度和可靠性的一种方法。

## intro
文件系统是现代操作系统的中心部分，并且被期望又快又可靠。然而问题还是会发生，机器会出人意料的发生故障，由于硬件软件或是电源的问题。

在意料之外的重启过后，电脑可能要花上一些时间来进行文件系统的恢复。随着磁盘容量的增大，这个时间也会成为一个严肃的问题，让系统脱机一个或几个小时进行磁盘的扫描，检查以及修复。尽管磁盘的速度也在逐年提高，但是相比于容量的增速来讲，磁盘的读写增速还是太温和了。不幸的是，每两倍的磁盘容量就会导致两倍的恢复时间，如果使用的是传统的文件系统检查技术。

由于系统可靠性很重要，这个时间不能节省，所以我们需要一种机制。可以避免每次重启时都需要恢复阶段.

## 文件系统里有啥
在文件系统中我们需要什么功能？有许多必须的功能是文件系统服务的操作系统决定的。文件系统暴露给应用的方式是操作系统通常要求文件名遵循某些约定，并且文件具有某些属性，这些属性以特定方式解释。

然而，文件系统内部则不必过多受限，实现者在设计文件系统时有一定的自由。数据在磁盘的布局(相应的如果文件系统不是本地的，则是网络协议),内部缓存的细节，和调度磁盘IO使用的算法，这些都是可以被修改而又不违反文件系统的应用接口规范的。

选择一种设计的原因有许多。与旧文件系统的兼容性可以是其中之一:`linux`提供了`UMSDOS` 文件系统，它在`MSDOS`磁盘文件结构之上实现了`POSIX`文件系统的语义.

在解决`linux`中长时间的文件系统恢复问题上，必须要牢记许多目标
- 新文件系统的性能不应牺牲过多
- 对于已有应用的兼容性不能被破坏
- 文件系统的可靠性不可妥协

## 文件系统的可靠性
当我们谈论到文件系统时，有许多问题文重要。由于这个特定项目，我们主要感兴趣的是可靠性，我们可以恢复崩溃文件系统的内容，我们可以确定几个方面:

`Preservation`:数据在崩溃前是稳定的，永远不应被破坏。显然，在崩溃时写入的文件无法保证其完整性，但是许多已经在磁盘上的文件是安全的，也不会被恢复系统所触及。

`Predictability`:由于必须恢复而进入的故障模式是客预测的，以便我们可靠地进行恢复

`Atomicity`:许多文件系统操作需要大量的分开的IO去完成。比如，重命名一个文件从一个目录到另一个目录. 在恢复后如果文件系统操作是全完成或者全部不完成,则恢复是原子的.

## 已有的实现
`ext2fs`提供了保留恢复，但他不是原子的，也不可预测。可预测性实际上是一个比一眼看上去更复杂的属性。为了能够可预测的在崩溃后进行处理，恢复阶段必须能够确定文件系统在遇到表示磁盘上未完成操作的不一致时正在执行的操作。总的来说，这需要当单个更新操作改变了多个磁盘块的时候文件系统能以可预测的顺序写磁盘。

有许多方法实现这种磁盘写入排序。最简单的方法就是等待第一次写完成，再提交下一个到设备驱动，即 同步元数据更新 方法。这种方法被`BSD`的`Fast File System`采用，出现在4.2的版本中，启发了许多`Unix`文件系统，包括`ext2fs`.

然而，同步元数据更新的一大缺点就是他的性能。如果文件系统操作需要等待磁盘IO完成，我们就不能批量进行一个磁盘上的多个文件系统更新。比如,如果我们创建了许多目录`entry`在一个目录块里，同步更新法需要我们独立多次写目录块到磁盘。

~~以上是逐句翻译~~


有一种方法可以解决这种性能问题。比如保证在内存中设置磁盘缓冲区并且保证磁盘缓冲区写回的顺序就可以保证磁盘写入顺序而不需要等待实际的IO完成。这种方法在写回块之前需要等待该块之前的磁盘块全部写入磁盘后，才可以写入。这种方法叫做延迟顺序写回技术.


但是这种方法容易导致循环依赖的问题，比如以下磁盘块的写入顺序: 1,2,1. 块号2的需要等待所有块号1的写入完成，但是有一个块号1操作在2的后面，导致1,2都无法写入。

`Ganger`的软更新避开了这个问题。在第一次将缓冲区写入磁盘时，如果更新有未完成的依赖关系，则会回滚缓冲区到特定的更新(我的理解是撤销一部分最新的更新，以便消除依赖，等待写入后再进行恢复更新)。

这些方法都需要扫描整个磁盘，很费时间。

在不牺牲可靠性和可预测性的情况下恢复文件系统是可能的。可以通过保证文件系统对磁盘更新的原子性来实现。文件系统将批量数据写入磁盘，这些数据在最终提交事务之前只存在于内存中，并没有真正写入磁盘。提交事务后，磁盘写入完成，磁盘上的批量数据得到更新。这里假设单个磁盘块的写入是原子的。

有许多方法可以实现原子性。日志结构化的文件系统通过将所有文件系统数据以日志写入磁盘的方式来实现原子性。当磁盘的元数据更新时，更新被记录在磁盘上用于日志保留的单独区域，完成的文件系统事务将提交数据到磁盘日志区中。事务是原子的，一个事物完成会消除磁盘日志区中的标记，则重启后不会进行恢复，否则默认将恢复日志区存放的操作恢复到文件系统中。

xv6的日志实现方式:
所有对文件的操作都由`begin_op`与`end_op`包裹起来，对文件的操作都会日志区(由于磁盘块在内存中存放在`bcache`里，所以实际上写在了磁盘缓冲区里).在最后`end_op`后进行提交事务，会将`bcache`里的日志块写到磁盘当中，写日志块的最后写入日志头(日志头就一个块，所以是原子的写入，如果日志头没有写入就崩溃了，则无法恢复缓存在日志块的操作，不过这是可接受的，保证了事务的原子性).

## 新的实现方法
为了消除崩溃后的系统恢复时间，可以选择日志化的方式。由于日志块很小，所以是恢复时间相比扫描整个磁盘要小得多。而且日志将临时数据存放在日志中，独立于磁盘上的永久数据和元数据。所以日志化方式不要求文件系统存储数据的方式。因此，日志化的方式可以用在`ext2fs`文件系统中。我们可以向`ext2fs`中添加日志化的特性。

## 事务剖析

日志化文件系统的核心是事务。任何对文件系统的请求都会产生一个事务，事务包含所有要在磁盘上修改的数据。

事务中还要包括读取文件系统的内容，因此事务有了顺序，我们必须在某些写操作之前进行读操作。事物之间也有了依赖性，某些事务必须按顺序进行。

最后，我们在执行分配新块的事务之前，必须将事务创建的数据块写入了磁盘，也就是在分配块给文件的事务前，必须已经将数据写入新块内了。(这一小段英文写的也很含糊，我猜测是这个意思)

## 事务合并
数据库事务和文件系统的事务之间的区别
1. 文件系统的事务没有显式的终止
2. 文件系统的事务很短，事务内的操作较少，一般不会有很多操作的事务.

因此可以对事务模型简化，我们对一个块的多次更新可以合并成一次块更新在事务中.

## 磁盘表示
一节不重要，讲了日志化的`ext2fs`与旧的`ext2fs`是兼容的。

## 文件系统日志的格式
我们向日志写入3种不同的数据块:元数据块，描述符块和头块
无论元数据的改动多么小，我们都需要整个的块来记录更新，不过日志块的写入是顺序的所以速度很快，对磁盘控制器友好。

`linux`内核提供了一种很有效的机制，类似`xv6`的实现，`buffer_head`描述了磁盘缓冲区，包括数据要写到哪个磁盘块的信息。我们可以向`buffer_head`内写入内容，之后提交IO进行修改。

描述符块是描述日志块元数据的日志块，当元数据块写到日志里时，要记录元数据块存放在哪些磁盘块内，这样恢复时就知道要往哪里恢复了(类似`xv6`中的日志头).

日志文件包含一些固定的位置的头块(日志块组成一个循环数组),头块记录了日志当前的头和尾，以便恢复时找到开头的日志块。

## 日志的提交和检查点
某些时候比如操作完成或日志空间不足，我们需要提交当前事务。事务提交后我们仍没有进行写入磁盘，仍然需要进行跟踪。

完全提交并完成事务,需要:
(论文写真的看不明白，不知道哪里是内存的日志块缓存，哪里是磁盘上的真正日志块)
1. 关闭事务，创建新事务用来记录以后的文件系统操作
2. 事务flush到磁盘的日志块
3. 等待事务中未完成的文件系统操作(都是磁盘)
4. 等待未完成的事务更新日志(磁盘)
5. 更新日志头块，记录日志块的头尾
6. 日志真正写入磁盘后会清除一些标志，表示写入磁盘，当最后一个日志块写入磁盘后，下一个事务才能使用刚刚占用的日志块。

## 事务冲突

为了性能，提交事务的时候不会暂停文件系统的更新，因此可能会产生一些冲突。

如果一个更新要访问被另一个属于上一个事务更新的元数据缓冲区，而且上一个事务正在提交。则发生了冲突，当前更新不可以去更新那个元数据缓冲区，因为正在提交事务。

如果只是想要读缓冲区，是没有问题的，我们可以直接读。
但是如果想写缓冲区，那么就有点麻烦了。有一个解决方法是将缓冲区拷贝到其他地方供新事务访问，当缓冲区拷贝结束前，旧事务不可以释放对应的日志空间。


## 现状和未来的工作

上述设计只需要对`ext2fs`进行少量修改就可以实现日志的功能了。

由于日志块总是连续的进行读写,而现代的磁盘io中，数据直接从主存到磁盘，不经过cpu和缓存，所以对cpu和总线很友好。


可以单独保留一块日志磁盘，将多个文件系统的日志记录到这个磁盘上，可以保持高吞吐量，也不会损失文件系统所在磁盘的bandwidth。
