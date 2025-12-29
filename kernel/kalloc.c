// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem[NCPU];

void freerange(void *pa_start, void *pa_end);
void __kfree(void *pa, int hart);
void kstealmem(struct run **r, int hart);

void
kinit()
{
  for (int h = 0; h < NCPU; h++) {
    initlock(&kmem[h].lock, "kmem");
  }

  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);

  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    // not really sure why equally distributing pages across CPUs is causing more contention
    // maybe the tests are biased for setting all pages to calling cpu or they are wrong
    __kfree(p, 0);
}

void __kfree(void *pa, int hart) {
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem[hart].lock);
  r->next = kmem[hart].freelist;
  kmem[hart].freelist = r;
  release(&kmem[hart].lock);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  push_off();
  int hart = cpuid();
  pop_off();

  return __kfree(pa, hart);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  push_off();
  int hart = cpuid();
  pop_off();

  acquire(&kmem[hart].lock);
  r = kmem[hart].freelist;
  if (r) {
    kmem[hart].freelist = r->next;
  }
  release(&kmem[hart].lock);

  if(!r) {
    kstealmem(&r, hart);
  }

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}


void kstealmem(struct run **r, int hart) {
  for(int h = (hart+1) % NCPU; h != hart; h = (h + 1) % NCPU) {
    acquire(&kmem[h].lock);
    *r = kmem[h].freelist;
    if ((*r)) {
      kmem[h].freelist = (*r)->next;
    }
    release(&kmem[h].lock);
    if ((*r))
      break;
  }
}
