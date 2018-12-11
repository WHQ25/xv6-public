#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct shm_page {
    uint id;
    char *frame;
    int refcnt;
  } shm_pages[64];
} shm_table;

void shminit() {
  int i;
  initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));
  for (i = 0; i< 64; i++) {
    shm_table.shm_pages[i].id =0;
    shm_table.shm_pages[i].frame =0;
    shm_table.shm_pages[i].refcnt =0;
  }
  release(&(shm_table.lock));
}

int shm_open(int id, char **pointer) {
  int i;
  acquire(&(shm_table.lock));
  for (i = 0; i < 64; i++) {
    if (shm_table.shm_pages[i].id == id) {
      // Case 1, Shared memory segment already exist
      char *pa = shm_table.shm_pages[i].frame;
      uint sz = PGROUNDUP(myproc()->sz);
      mappages(myproc()->pgdir, (void *) sz, PGSIZE, V2P(pa), PTE_W | PTE_U);
      shm_table.shm_pages[i].refcnt++;
      myproc()->sz = sz + PGSIZE;
      *pointer = (char *) V2P(pa);
      release(&(shm_table.lock));
      return V2P(pa);
    }
  }
  // Case 2, Share memory segment doesn't exist
  for (i = 0; i < 64; i++) {
    if (shm_table.shm_pages[i].frame == 0) {
      shm_table.shm_pages[i].id = id;
      shm_table.shm_pages[i].frame = kalloc();
      shm_table.shm_pages[i].refcnt = 1;
      char *pa = shm_table.shm_pages[i].frame;
      uint sz = PGROUNDUP(myproc()->sz);
      mappages(myproc()->pgdir, (void *) sz, PGSIZE, V2P(pa), PTE_W | PTE_U);
      shm_table.shm_pages[i].refcnt++;
      myproc()->sz = sz + PGSIZE;
      *pointer = (char *) V2P(pa);
      release(&(shm_table.lock));
      return V2P(pa);
    }
  }
  return -1; //something is wrong
}

int shm_close(int id) {
  if (id < 0 || id > 64) {
    return -1; // invalid id
  }
  acquire(&(shm_table.lock));
  if (--shm_table.shm_pages[id].refcnt == 0) {
    shm_table.shm_pages[id].id = 0;
    shm_table.shm_pages[id].frame = 0;
  };
  release(&(shm_table.lock));
  return 0;
}
