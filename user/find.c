// Write a simple version of the UNIX find program: 
// find all the files in a directory tree with a specific name.
//Your solution should be in the file user/find.c.

// Some hints:

// Look at user/ls.c to see how to read directories.
// Use recursion to allow find to descend into sub-directories.
// Don't recurse into "." and "..".
// Changes to the file system persist across runs of qemu; 
// to get a clean file system run make clean and then make qemu.
// You'll need to use C strings. Have a look at K&R (the C book), for example Section 5.5.
// Note that == does not compare strings like in Python. Use strcmp() instead.
// Add the program to UPROGS in Makefile.
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// get filename from path
char* get_filename(char *path) {
    char *p;
    for (p=path+strlen(path); p>=path && *p != '/'; p--);
    p++;
    return p;
}

// 参考ls.c，递归查找目录，判断文件名是否相同
void find(char *path, char *name) {
    char buf[512], *p;
    int fd;
    struct stat st; // st 变量是一个 struct stat 类型的结构体，用于存储文件信息。
    struct dirent de; // de 变量是一个 struct dirent 类型的结构体，用于存储每个目录项的信息。

    // open会返回最小的fd，如果fd < 0，说明打开失败
    if ((fd=open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    // fstat会将文件信息存储在st中
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
    case T_FILE:
        // 如果是文件类型，则判断文件名是否相同
        p = get_filename(path);
        if (!strcmp(p, name))
            printf("%s\n", path);
        break;
    
    case T_DIR:
        // 如果是目录类型，则递归查找此目录，直到找到文件
        // 这里是核心代码，重点是目录也是个fd，相当于目录的fd中存储的是这样的结构
        // struct dirent dirents[] = {
        //     {1, "file1.txt"},
        //     {2, "file2.txt"},
        //     {3, "subdir"}
        // };
        // 那么我们就可以通过read读取目录下的文件
        // 这段代码通过 read 函数从目录文件描述符 fd 中读取目录项，并将其存储到 de 变量中。
        while(read(fd, &de, sizeof(de)) == sizeof(de)) {
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("find: path too long\n");
                break;
            }
            // 判断inode号
            if (de.inum == 0)
                continue;
            // 就跟ls一样，我们会看到.和..这两个文件出现，但是当前目录和父目录都是应该跳过的
            // 否则就会无限递归
            if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
                continue;
            // 复制path
            strcpy(buf, path);
            // p指向path的末尾'\0'
            p = buf + strlen(buf);
            // 末尾添加'/'，然后移动p指针
            *p++ = '/';
            // 将目录项的名称（de.name）复制到 p 指向的位置，复制的长度为 DIRSIZ。
            // 复制完成后，p 仍然指向原来的位置，只是 p 后面的内容被更新为 de.name 的内容。
            memmove(p, de.name, DIRSIZ);
            // 在 p 后面的第 DIRSIZ 位添加字符串终止符 \0，确保 buf 中的路径字符串以 \0 结尾。
            // 如果 de.name 的长度小于 DIRSIZ，那么在 de.name 之后的部分已经有 \0 存在，这样不会影响路径字符串的正确性。
            // 如果 de.name 的长度等于或大于 DIRSIZ，则文件名会被截断，但路径字符串仍然是有效的。
            p[DIRSIZ] = '\0';
            find(buf, name);
        }
        break;
    }
    close(fd);
    return;
}

void main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find <path> <name>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}