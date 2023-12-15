# how debuggers work?

## p1 basics

`ptrace`，调用这个函数可以使进程被另外的进程跟踪，任何发送给被追踪进程的信号都可以被追踪他的进程通过wait知晓。
被追踪进程之后调用exec函数也会在新程序运行前，产生SIGTRAP给追踪进程一个获得控制的机会.

```c
int main(int argc, char** argv)
{
    pid_t child_pid;

    if (argc < 2) {
        fprintf(stderr, "Expected a program name as argument\n");
        return -1;
    }

    child_pid = fork();
    if (child_pid == 0)
        run_target(argv[1]);
    else if (child_pid > 0)
        run_debugger(child_pid);
    else {
        perror("fork");
        return -1;
    }

    return 0;
}
void run_target(const char* programname)
{
    procmsg("target started. will run '%s'\n", programname);

    /* Allow tracing of this process */
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
        perror("ptrace");
        return;
    }

    /* Replace this process's image with the given program */
    execl(programname, programname, 0);
}
void run_debugger(pid_t child_pid)
{
    int wait_status;
    unsigned icounter = 0;
    procmsg("debugger started\n");

    /* Wait for child to stop on its first instruction */
    wait(&wait_status);

    while (WIFSTOPPED(wait_status)) {
        icounter++;
        /* Make the child execute another instruction */
        if (ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0) < 0) {
            perror("ptrace");
            return;
        }

        /* Wait for child to stop on its next instruction */
        wait(&wait_status);
    }

    procmsg("the child executed %u instructions\n", icounter);
}
```

子进程执行`ptrace(PTRACE_TRACEME,0,0,0)`,让父进程追踪自己，然后执行`execl`时会向父进程发送`SIGTRAP`,并暂停自己的运行，等待父进程的接管。

之后父进程进入`while` 调用`ptrace(PTRACE_SINGLESTEP)`,告知OS请重启子进程并执行到下一条指令停止

循环在子进程的状态不是STOP的时候跳出。

上述方法单步运行调试的是整个进程，包括了main函数运行前后的代码。




## breakpoint
trap，软中断

中断：外部的异步信号被cpu接收，cpu保存现在的状态，跳转到中断处理函数执行，之后再返回保存的位置继续执行

软中断：软件指令触发的中断而已

int3：trap to debugger


在dbg中通过int3设置断点:
将你想停在的位置的指令的开头一个byte替换为cc
程序执行到cc时，会将cc替换为原来程序机器码，同时pc往回走一条指令，重新执行这条替换过的指令
允许用户通过一些方式与进程交互，比如可以查看栈的内容，进程的变量，寄存器等

如果程序要继续进行调试，断点会被放回，等待下次触发，如果用户要取消断点，就不必将cc放回了

cc机器码通过位操作写入指令
