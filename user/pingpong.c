#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Write a program that uses UNIX system calls to "ping-pong" a byte between two processes 
// over a pair of pipes, one for each direction.
// The parent should send a byte to the child; the child should print "<pid>: received ping", 
// where <pid> is its process ID, write the byte on the pipe to the parent, and exit;
// the parent should read the byte from the child, print "<pid>: received pong", and exit.
// Your solution should be in the file user/pingpong.c.

// Some hints:
// Use pipe to create a pipe.
// Use fork to create a child.
// Use read to read from the pipe, and write to write to the pipe.
// Use getpid to find the process ID of the calling process.
// Add the program to UPROGS in Makefile.
// User programs on xv6 have a limited set of library functions available to them. You can see the list in user/user.h; the source (other than for system calls) is in user/ulib.c, user/printf.c, and user/umalloc.c.
int main(void) {
    // Use pipe to create a pipe.
    int p[2];
    if(pipe(p) < 0) {
        fprintf(2, "failed to create pipe\n");
        exit(1);
    }
    char c = 'a';
    // child (receive -> send)
    if (fork() == 0) {
        if (read(p[0], &c, 1) != 1) {
            fprintf(2, "failed to read in child\n");
            exit(1);
        }
        close(p[0]);
        printf("%d: received ping\n", getpid());
        if (write(p[1], &c, 1) != 1) {
            fprintf(2, "failed to write in child\n");
            exit(1);
        }
        close(p[1]);
        exit(0);
    }
    // parent (send -> receive)
    if (write(p[1], &c, 1) != 1) {
        fprintf(2, "failed to write in parent\n");
        exit(1);
    }
    close(p[1]);
    wait(0);
    if (read(p[0], &c, 1) != 1) {
        fprintf(2, "failed to read in parent\n");
        exit(1);
    }
    printf("%d: received pong\n", getpid());
    close(p[0]);
    exit(0);
}