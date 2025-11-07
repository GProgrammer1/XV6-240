#include "types.h"

// Spinning ticket lock.
typedef struct ticketlock {
  uint ticket;        // current ticket number being served
  uint turn;          // next ticket number to be given
  struct proc *proc;  // process currently holding the lock
} ticketlock;

void initlock_t(struct ticketlock *lk);
void acquire_t(struct ticketlock *lk);
void release_t(struct ticketlock *lk);