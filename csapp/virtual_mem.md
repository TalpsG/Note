# virtual mem
## page  table

每一个虚拟页根据页表项 page table entry 分为三种状态
1. 已分配且已经缓存在内存中,有效位为1,页表项内容为该页面的起始物理地址
2. 已分配但未缓存，页面还在磁盘中，有效位为0
3. 未分配，页表项为NULL,虚拟页中还未有内容,有效位为0

## mm_struct 
mm_struct 存放了第一级页表的地址以及一个mmap(vm_area_struct类型的结构体的指针),mmap指向的是一条虚拟内存分配的链表，每一个元素都有占用的起始地址，结束地址，权限等等信息。

## 内存映射

linux将虚拟内存区域与磁盘上的对象联系起来

### 共享对象

进程间可以有共享对象与私有对象。
#### 共享区域
存放共享对象的内存区域被称为共享区域，多个进程的共享区域映射的是同一块物理内存（因为共享对象也存放在该物理内存处），一个进程对对象的操作对其他进程可见


#### 私有区域
进程对私有对象的操作对其他进程是不可见的

#### 写时复制
当两个进程将同一个私有对象映射到自己的虚拟内存时，他们会先映射到同一块物理内存，且对应的这块虚拟内存空间被设置为写时复制.
当进程对该虚拟内存区域写时，会进入保护故障，故障处理程序注意到进程要写私有的写时复制区域，所以他会在物理内存中创建一个该页的副本，然后更改页表中对应表项，并设置允许写，然后返回到写操作继续执行。

## fork()

当进程在调用fork后子进程会复制父进程的mm_struct，将父子进程中的页面都设为**只读**,并修改区域为私有写时复制，这样既节省了物理内存，也可以保持两个进程的独立性。


## execve()
execve 的调用在加载运行a.out的时候需要以下的步骤
1. 删除已存在的用户区域，也就是mmap指向的结构体链表
2. 映射私有区，为新程序创建新的结构区域，bss为command-zero page ，代码区和数据区为data段和text段，这些区域都是私有写时复制的。新创建的堆和栈区也都是command-zero page
3. 映射共享区域，如果动态链接了libc中的对象，对象会被映射到共享区
4. 设置pc

## mmap
```c
void *mmap(void addr[.length], size_t length, int prot, int flags,
int fd, off_t offset);
```
**当mmap中的fd取为-1时，表示不映射任何文件对象，mmap分配一块匿名虚拟内存**

## 编写一个c程序，用mmap将一个任意大小的磁盘文件复制到stdout上
```c
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
void mmapcopy(int fds, int fdd, int size) {
  char *p = (char *)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fds, 0);
  write(1, p, size); // 向fd的文件内写入
  munmap(p, size);
}
int main(int argc, char *argv[]) {
  int fd = open(argv[1], O_RDONLY, 0);
  if (fd == -1) {
    printf("no file is this name\n");
    return 0;
  }
  struct stat statf;
  fstat(fd, &statf);
  mmapcopy(fd, 1, statf.st_size);
  return EXIT_SUCCESS;
}
```

## mem allocator

栈在高地址向低生长，堆则相反，自低向高生长。

内存分配器分为显式与隐式，c/c++是显式分配，而java等高级语言则为隐式分配


### malloc

#### 隐式链表法

内存块前后都有一个块(4Byte大小)用于记录该块的信息，分别叫header和footer,内容相同，footer是为了让本内存块可以检索到上一个内存块的信息而设置的。

#### 显式链表法

内存块前有一个块记录大小等信息以及prev和next内存块的指针。

#### 空闲链表法

1. 简单法：将堆划分为多个不同大小的块，大小相同的在一条链表上，分配时不做分割，会产生内部碎片
2. 分离适配：与简单发类似，分配器维护多个空闲链表，但是在分配的时候会对空闲块进行分割，将剩余的部分插入合适的空闲链表中
3. 伙伴系统buddy system: 


### gc 垃圾回收

维护一颗二叉树，从根节点（未被其他指针指向的已分配块）开始标记被指向的内存块，通过已分配块二叉树去判断指针是否指向已分配块，标记完成后，回收未标记的已分配块。


## homework

1. mmap实现对文件的修改

```c
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
int main() {
  int fd = open("/home/talps/git_repo/note/csapp/hello.txt", O_RDWR);
  struct stat temp;
  fstat(fd, &temp);
  char *p = (char *)mmap(NULL, temp.st_size, PROT_WRITE | PROT_READ, MAP_SHARED,
                         fd, 0); //MAP_SHARED 会修改文件
  *p = 'J';
  return 0;
}
```







