#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "users.h"
#include "ticketlock.h"

#define SYSCALLS_COUNT 25
int syscalls_count[SYSCALLS_COUNT] = {0};

int
sys_fork(void)
{
  return fork();
}

int
sys_clone(void)
{
  int fn, arg, stack;

  if(argint(0, &fn) < 0 || argint(1, &arg) < 0 || argint(2, &stack) < 0)
    return -1;

  return clone((void (*)(void*))fn, (void*)arg, (void*)stack);
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_join(void)
{
  void **stack;

  if(argptr(0, (char**)&stack, sizeof(void*)) < 0)
    return -1;

  return join(stack);
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
// function responsible of authenticating the user
int
sys_authenticate(void)
{
    char *username, *password;
    int i, j;
    int match;
    
    if(argstr(0, &username) < 0 || argstr(1, &password) < 0)
        return -1;
    
    // Check credentials
    for(i = 0; i < NUMUSERS; i++) {
        // Check username match
        match = 1;
        for(j = 0; users[i].username[j] != '\0' || username[j] != '\0'; j++) {
            if(users[i].username[j] != username[j]) {
                match = 0;
                break;
            }
        }
        
        if(!match)
            continue;
            
        // Check password match
        match = 1;
        for(j = 0; users[i].password[j] != '\0' || password[j] != '\0'; j++) {
            if(users[i].password[j] != password[j]) {
                match = 0;
                break;
            }
        }
        
        if(match)
            return 0; // Success
    }
    
    return -1; // Authentication failed
}

int
sys_count(void)
{
  int a;
  if(argint(0, &a) > 0)
  {
    return syscalls_count[a - 1];
  }

  int total = 0;
  for (unsigned i = 1; i <= SYSCALLS_COUNT; i++)
  {
    total += syscalls_count[i - 1];
  }
  return total;
}

int sys_initlock_t(void)
{
  struct ticketlock *tl;
  if (argptr(0, (char**)&tl, sizeof(struct ticketlock*)) < 0)
  {
    return -1;
  }

  initlock_t(tl);
  return 0;
}

int sys_acquire_t(void)
{
  struct ticketlock *tl;
  if (argptr(0, (char**)&tl, sizeof(struct ticketlock*)) < 0)
  {
    return -1;
  }

  acquire_t(tl);
  return 0;
}

int sys_release_t(void)
{
  struct ticketlock *tl;
  if (argptr(0, (char**)&tl, sizeof(struct ticketlock*)) < 0)
  {
    return -1;
  }

  release_t(tl);
  return 0;
}