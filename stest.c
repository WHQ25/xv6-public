#include "types.h"
#include "user.h"

int main(int argc, char *argv[])
{

    int test1(void);
    test1();


   
    exit(0);
}

int test1(void){

   
    int pid, ret_pid, exit_status;
    int i,j,k;
    int place = 0;

    setpriority(0);
    for (i = 0; i <  3; i++) {
        pid = fork();
        if (pid > 0 ) {
            continue;}
        else if ( pid == 0) {
            printf(1, "\n Hello! this is child# %d and I have priority of  %d \n",getpid(),1+i);
            setpriority(1+i);
            for (j=0;j<50000;j++) {
                for(k=0;k<10000;k++) {
                    asm("nop"); }}
            
            printf(1, "\n child# %d has finished! I began with priority %d  \n",getpid(),1+i,place);
            exit(0);
        }
        else {
            printf(2," \n Error \n");
            exit(-1);
        }
    }



    if(pid > 0) {
        for (i = 0; i <  3; i++) {
            ret_pid = wait(&exit_status);
            //printf(1,"\n This is the parent: child with PID# %d has finished with status %d \n",ret_pid,exit_status);
        }
        printf(1,"\n   \n");
    }

    if (ret_pid == 0) {} // add this line to avoid ret_pid not used warning

    return 0;
}