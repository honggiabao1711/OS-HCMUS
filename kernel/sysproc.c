#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
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
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    if(addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
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

  argint(0, &pid);
  return kkill(pid);
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

// them vao day
uint64
sys_trace(void)
{
    int mask;

    argint(0, &mask);

    myproc()->tracemask = mask;

    return 0;
}
uint64
sys_procinfo(void)
{
  int pid;
  uint64 addr;
  struct proc *p;
  struct procinfo k_info;
  extern struct proc proc[NPROC];

  argint(0, &pid);
  argaddr(1, &addr);

  int found = 0;
  for (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if(p->pid == pid) {
      k_info.pid = p->pid;
      k_info.state = p->state;
      k_info.sz = p->sz;
      safestrcpy(k_info.name, p->name, sizeof(k_info.name));

      if(p->parent) k_info.ppid = p->parent->pid;
      else k_info.ppid = 0;

      found = 1;
      release(&p->lock);
      break;
    }
    release(&p->lock);
  }

  if(!found) return -1;

  if(copyout(myproc()->pagetable, addr, (char *)&k_info, sizeof(k_info)) < 0)
    return -1;

  return 0;
}
