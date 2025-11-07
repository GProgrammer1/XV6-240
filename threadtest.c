#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "param.h"

#define NTHREADS 3
#define STACK_SIZE 4096

struct stackinfo {
  void *raw;
  void *aligned;
};

static int
allocstack(struct stackinfo *info)
{
  char *raw;

  raw = malloc(STACK_SIZE * 2);
  if(raw == 0)
    return -1;

  info->raw = raw;
  info->aligned = (void*)(((uint)raw + (STACK_SIZE - 1)) & ~(STACK_SIZE - 1));
  return 0;
}

static void
worker(void *arg)
{
  int id = (int)arg;
  int i;

  for(i = 0; i < 5; i++){
    printf(1, "thread %d iteration %d\n", id, i);
    sleep(5);
  }

  printf(1, "thread %d exiting\n", id);
  exit();
}

int
main(void)
{
  int i, j;
  int pid;
  int pids[NTHREADS];
  struct stackinfo stacks[NTHREADS];
  void *stackptr;

  printf(1, "threadtest: starting %d threads\n", NTHREADS);

  for(i = 0; i < NTHREADS; i++){
    if(allocstack(&stacks[i]) < 0){
      printf(2, "threadtest: stack allocation failed\n");
      exit();
    }

    pids[i] = clone(worker, (void*)i, stacks[i].aligned);
    if(pids[i] < 0){
      printf(2, "threadtest: clone failed\n");
      exit();
    }
  }

  for(i = 0; i < NTHREADS; i++){
    stackptr = 0;
    pid = join(&stackptr);
    if(pid < 0){
      printf(2, "threadtest: join failed\n");
      exit();
    }

    for(j = 0; j < NTHREADS; j++){
      if(stackptr == stacks[j].aligned && stacks[j].raw){
        free(stacks[j].raw);
        stacks[j].raw = 0;
        break;
      }
    }

    printf(1, "threadtest: joined thread pid %d\n", pid);
  }

  printf(1, "threadtest: done\n");
  exit();
}

