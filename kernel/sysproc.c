#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// 新增trace系统调用，参考sys_kill，读取mask参数，并记录到proc结构体中
uint64 sys_trace(void) {
  int mask;
  // get mask from user space: 这里从用户空间获取的是mask，不是syscall number
  if (argint(0, &mask) < 0) {
    return -1;
  }
  myproc()->trace_mask = mask;
  return 0;
}

// 新增sysinfo系统调用，从内核空间获取sysinfo，然后拷贝到用户空间
// The system call takes one argument: a pointer to a struct sysinfo (see kernel/sysinfo.h). 
// The kernel should fill out the fields of this struct: the freemem field should be set to the number of bytes of free memory, 
// and the nproc field should be set to the number of processes whose state is not UNUSED. We provide a test program sysinfotest; 
// you pass this assignment if it prints "sysinfotest: OK".
uint64 sys_sysinfo(void) {
  // user space调用sysinfo会传入struct sysinfo *，我们需要获取这个user space的virtual address
  // 把从kernel space中获取到的sysinfo，copy到user space中
  uint64 addr;
  if (argaddr(0, &addr) < 0)
    return -1;
  // get sysinfo from kernel space
  struct sysinfo info;
  info.freemem = freemem();
  info.nproc = usedproc();
  // Copy from kernel to user.
  // Copy len bytes from src to virtual address dstva in a given page table.
  if (copyout(myproc()->pagetable, addr, (char*)&info, sizeof(info)) < 0)
    return -1;
  return 0;
}