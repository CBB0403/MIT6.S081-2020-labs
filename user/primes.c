// Write a concurrent version of prime sieve using pipes.
// This idea is due to Doug McIlroy, inventor of Unix pipes. 
// The picture halfway down this page and the surrounding text explain how to do it.
// Your solution should be in the file user/primes.c.

// Your goal is to use pipe and fork to set up the pipeline.
// The first process feeds the numbers 2 through 35 into the pipeline.
// For each prime number, you will arrange to create one process that reads from its left neighbor 
// over a pipe and writes to its right neighbor over another pipe. 
// Since xv6 has limited number of file descriptors and processes, the first process can stop at 35.

// Some hints:

// Be careful to close file descriptors that a process doesn't need, 
// because otherwise your program will run xv6 out of resources before the first process reaches 35.

// Once the first process reaches 35, it should wait until the entire pipeline terminates, 
// including allchildren, grandchildren, &c. Thus the main primes process should only exit after 
// all the output has been printed, and after all the other primes processes have exited.

// Hint: read returns zero when the write-side of a pipe is closed.
// It's simplest to directly write 32-bit (4-byte) ints to the pipes, rather than using formatted ASCII I/O.

// You should create the processes in the pipeline only as they are needed.

// Add the program to UPROGS in Makefile.

// 要求實作一個並行的質數篩選器，範圍從 2~35，MIT 提供 Bell Labs and CSP Threads 連結給我們參考，其概念簡介如下：
// 每個 block 的第一個數字 N 一定是質數，因為除了 1 跟自己，沒有其他因數
// 剩餘的數字除上 N，若不可整除代表為質數的候選者，將數字向後傳遞
// 重複上述步驟，直到 block 沒有數字可以向後傳為止
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void new_proc(int p[2]) {
    int prime, n;
    close(p[1]);
    // exit condition: read returns zero when the write-side of a pipe is closed.
    if (!read(p[0], &prime, 4)) {
        exit(0);
    }
    // first number must be prime
    printf("prime %d\n", prime);

    // create new pipe and child process
    // - child process recursively call new_proc
    // - parent process send all numbers that are not multiples of prime
    int newP[2];
    pipe(newP);
    if (fork() == 0) {
        new_proc(newP);
    }
    close(newP[0]);
    while(read(p[0], &n, 4)) {
        if (n % prime) {
            if (write(newP[1], &n, 4) != 4) {
                fprintf(2, "failed to write in child process\n");
                exit(1);
            }
        }
    }
    close(p[0]);
    close(newP[1]);
    wait(0);
    exit(0);
}

int main(void) {
	int p[2];
	pipe(p);
    // create child process, and the child process will create grandchild process
    // child process will read from its left neighbor over a pipe and write to its right neighbor over another pipe
	if (fork() == 0) {
        new_proc(p);
    }
    // first process, send all numbers from 2 to 35
    close(p[0]);
    for (int i = 2; i <= 35; i++) {
        if (write(p[1], &i, 4) != 4) {
            fprintf(2, "first process failed to write %d into the pipe\n", i);
            exit(1);
        }
    }
    close(p[1]);
    wait(0);
    exit(0);
}