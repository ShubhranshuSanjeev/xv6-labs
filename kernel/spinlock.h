#include "param.h"
#include "stdint.h"

// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
#ifdef LAB_LOCK
  int nts;
  int n;
#endif
};

#ifdef LAB_LOCK
// Reader-writer lock.

enum RWLockState {
  Reading,
  WaitingWriters,
  // Writing,
};

// #define MAX_READERS 1073741823
#define MAX_READERS 99999999
#define MAX_WRITERS 2

struct rwspinlock {
  enum RWLockState state;

  int64_t shared_count; // even if only readers, odd if writers waiting
  int active_readers;
  int waiting_writers; // max is 2, to  
  struct spinlock *sync_writers; // lock to sync writers updating wait_state

  char *name;
  struct cpu *writer;

  // TODO: add some kind of wakeup for readers and writer
};
#endif
