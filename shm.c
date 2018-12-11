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
    for (i = 0; i< 64; i++) {

        if (shm_table.shm_pages[i].id == id)
        {
            mappages(pointer, sz, PGSIZE, shm_table.shm_pages[i].frame, PTE_W|PTE_U );
            shm_table.shm_pages[i].refcnt = shm_table.shm_pages[i].refcnt + 1;

            //*pointer=(char *)va;
            release(&(shm_table.lock));

            return sz;
        }
    }

    for (i = 0; i< 64; i++) {

        if(shm_table.shm_pages[i].frame == 0)
        {
            shm_table.shm_pages[i].id = id;
            shm_table.shm_pages[i].frame = kalloc();
            shm_table.shm_pages[i].refcnt = 1;
            mappages(pointer, sz, PGSIZE, shm_table.shm_pages[i].frame, PTE_W|PTE_U );
            release(&(shm_table.lock));

            return sz;
        }

    }


 //added to remove compiler warning -- you should decide what to return
}


int shm_close(int id) {
//you write this too!
    int i;
    
    acquire(&(shm_table.lock));
    for (i = 0; i< 64; i++) {

        if (shm_table.shm_pages[i].id == id)
        {
            shm_table.shm_pages[i].id =0;
            shm_table.shm_pages[i].frame =0;
            shm_table.shm_pages[i].refcnt =0;
        }
    }
    release(&(shm_table.lock));

return 0; //added to remove compiler warning -- you should decide what to return
}
