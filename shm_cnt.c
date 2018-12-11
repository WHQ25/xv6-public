#include "types.h"
#include "stat.h"
#include "user.h"
#include "uspinlock.h"
#include "x86.h"

struct shm_cnt {
   struct uspinlock lock;
   int cnt;
};

void
uacquire(struct uspinlock *lk)
{
    // The xchg is atomic.
    while(xchg(&lk->locked, 1) != 0)
        ;

    // Tell the C compiler and the processor to not move loads or stores
    // past this point, to ensure that the critical section's memory
    // references happen after the lock is acquired.
    __sync_synchronize();
}

void urelease (struct uspinlock *lk) {
    __sync_synchronize();

    // Release the lock, equivalent to lk->locked = 0.
    // This code can't use a C assignment, since it might
    // not be atomic. A real OS would use C atomics here.
    asm volatile("movl $0, %0" : "+m" (lk->locked) : );
}

int main(int argc, char *argv[])
{
int pid;
int i=0;
struct shm_cnt *counter;
  pid=fork();
  sleep(1);

//shm_open: first process will create the page, the second will just attach to the same page
//we get the virtual address of the page returned into counter
//which we can now use but will be shared between the two processes
  
shm_open(1,(char **)&counter);
 
//  printf(1,"%s returned successfully from shm_open with counter %x\n", pid? "Child": "Parent", counter); 
  for(i = 0; i < 10000; i++)
    {
     uacquire(&(counter->lock));
     counter->cnt++;
     urelease(&(counter->lock));

//print something because we are curious and to give a chance to switch process
     if(i%1000 == 0)
       printf(1,"Counter in %s is %d at address %x\n",pid? "Parent" : "Child", counter->cnt, counter);
}
  
  if(pid)
     {
       printf(1,"Counter in parent is %d\n",counter->cnt);
    int status;
    wait(&status);
    } else
    printf(1,"Counter in child is %d\n\n",counter->cnt);

//shm_close: first process will just detach, next one will free up the shm_table entry (but for now not the page)
   shm_close(1);
   exit(0);
   return 0;
}
