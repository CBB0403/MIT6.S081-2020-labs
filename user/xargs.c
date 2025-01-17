// Write a simple version of the UNIX xargs program: read lines from the standard input and
// run a command for each line, supplying the line as arguments to the command. 
// Your solution should be in the file user/xargs.c.

// The following example illustrates xarg's behavior:
//     $ echo hello too | xargs echo bye
//     bye hello too
//     $

// Note that the command here is "echo bye" and the additional arguments are "hello too", 
// making the command "echo bye hello too", which outputs "bye hello too".
// Please note that xargs on UNIX makes an optimization where it will feed more than argument to 
// the command at a time. We don't expect you to make this optimization. 
// To make xargs on UNIX behave the way we want it to for this lab, 
// please run it with the -n option set to 1. For instance
//     $ echo "1\n2" | xargs -n 1 echo line
//     line 1
//     line 2
//     $

// Some hints:

// Use fork and exec to invoke the command on each line of input.
// Use wait in the parent to wait for the child to complete the command.
// To read individual lines of input, read a character at a time until a newline ('\n') appears.
// kernel/param.h declares MAXARG, which may be useful if you need to declare an argv array.
// Add the program to UPROGS in Makefile.
// Changes to the file system persist across runs of qemu;
// to get a clean file system run make clean and then make qemu.
// xargs, find, and grep combine well:
//   $ find . b | xargs grep hello

// will run "grep hello" on each file named b in the directories below ".".

// xargs 命令用于从标准输入读取一行或多行输入，并将每一行作为参数传递给指定的命令执行。
// 它的主要功能是将输入的每一行作为命令的参数来执行。
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

// xargs 函数创建一个子进程来执行命令。
// 使用 fork 创建子进程，如果是子进程，则调用 exec 执行命令。
// 父进程使用 wait 等待子进程完成。
void xargs(char *argv[]) {
    int pid, status;
    if ((pid=fork()) == 0) {
        exec(argv[0], argv);
        exit(1);
    }
    wait(&status);
    return;
}

// main函数主要处理输入的参数，把stdin的每一行作为参数传递给xargs函数。
// 本次实验没有要求优化xargs，接收传入'-n {lines}'来限制读取的行数
// 因此直接读取每一行，然后添加到
void main(int argc, char *argv[]) {
    // check the number of arguments
    int i, j;
    char c, buf[512], *xargv[MAXARG];
    if (argc < 2 || argc - 1 > MAXARG) {
        fprintf(2, "usage: xargs <cmd> {args}, # of args should be less than 32\n");
        exit(1);
    }

    // init xargv
    memset(buf, 0, sizeof(buf));
    for (i = 1; i < argc; i++)
        xargv[i - 1] = argv[i];
    for (; i < MAXARG ; i++)
        xargv[i] = 0;

    // 从标准输入读取字符，直到遇到换行符，将每一行作为参数传递给命令执行
    j = 0;
    while (read(0, &c, 1) > 0) {
        // 将读取的行存储在 buf 中，并在 buf 末尾添加字符串终止符 \0。
        if (c != '\n')
            buf[j++] = c;
        else {
            // 将 buf 作为最后一个参数传递给 xargv，然后调用 xargs 函数执行命令。
            if (j != 0) {
                buf[j] = '\0';
                xargv[argc - 1] = buf;
                xargs(xargv);
                j = 0;
            }
        }
    }
    // 如果输入结束时 buf 中还有未处理的内容，再次调用 xargs 函数执行命令。
    if (j != 0) {
        buf[j] = '\0';
        xargv[argc - 1] = buf;
        xargs(xargv);
    }
    exit(0);
}
