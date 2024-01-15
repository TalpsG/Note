# lazy page allocation
xv6向内核申请堆内存使用sbrk
可以让sbrk不分配物理内存，只记录已经分配的地址空间，pte是invalid的
## lazy allocation
修改trap.c 去响应pagefault

先改sbrk,仅仅增加sz就可以
然后修改trap.c 响应pagefault
修改uvmunmap：
1. 找不到pte是正常的，因为有些地址空间并未分配内存，也没有pte
2. pte无效也是正常的 因为有些并没有分配地址

注意:
1. 新建页面后要清0
2. unmap的时候注意有些情况可以跳过
3. pagefault判断地址大于stack小于heap
3. 系统调用使用sbrk的地址的时候，由于物理页并未分配，导致copyin或copyout等api从userspace向kernel 复制数据失败
但是不能为所有找不到pte的va创建页面，因为va也有违法的va。解决方法可以是在copy 的三个函数中判断va是否在sbrk的范围内(栈之上，堆之下)
在的话如果walk不到就分配


## 大致的实验步骤
1. 修改sbrk,仅仅改动sz即可
2. trap.c添加pagefault 的处理代码,需要判断地址是否在sbrk下面，userstack上面
3. 修改uvmunmap
    - 没找到pte就跳过，因为lazy的缘故，pte可能并未分配
    - 找到pte但是invalid也要跳过，因为只要能找到对应最后一级页表的pte，就一定能找到对应va的pte，所以还需要判断valid
4. 修改fork,copy uvm的时候也需要跳过上述的两个情况
5. 修改copyin等函数
    - 由于内核无法直接访问用户空间，需要walk查表，
    - 所以需要修改copyin等函数，在内部判断va是否在sbrk和stack之间
    - 在且pte找不到或者无效就要分配页面并且映射。


