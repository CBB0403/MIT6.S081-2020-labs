#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// argc (argument count)：表示命令行参数的数量
// argv (argument vector)：是一个指向字符串数组的指针，其中每个字符串都是一个参数
int main(int argc, char const *argv[]) {
	if (argc != 2)
	{
		fprintf(2, "Usage: sleep seconds\n");
		exit(1);
	}
	int time = atoi(argv[1]);
    // syscall
	sleep(time);
	
	exit(0);
}