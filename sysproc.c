#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  int status;
  argint(0, &status);
  exit(status);
  return 0;  // not reached
}

int
sys_wait(void)
{
  int *status;
  argptr(0, (char **) &status, sizeof(int));
  return wait(status);
}

int
sys_waitpid(void)
{
  int pid;
  int *status;
  int option;
  argint(0, &pid);
  argptr(1, (char **) &status, sizeof(int));
  argint(2, &option);
  return waitpid(pid, status, option);
}

int
sys_setpriority(void)
{
  int priority;
  argint(0, &priority);
  return setpriority(priority);
}

int
sys_getpriority(void)
{
  int pid;
  argint(0, &pid);
  return getpriority(pid);
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
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

int
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

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int sys_shm_open(void) {
  int id;
  char **pointer;

  if(argint(0, &id) < 0)
    return -1;

  if(argptr(1, (char **) (&pointer),4)<0)
    return -1;
  return shm_open(id, pointer);
}

int sys_shm_close(void) {
  int id;

  if(argint(0, &id) < 0)
    return -1;


  return shm_close(id);
}