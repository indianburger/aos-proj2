
#include <stdio.h>	 /* standard I/O routines.               */
#include <sys/types.h>   /* various type definitions.            */
#include <sys/ipc.h>     /* general SysV IPC structures          */
#include <sys/shm.h>	 /* shared memory functions and structs. */
#include <stdlib.h>
#include <string.h>
/* define a structure to be used in the given shared memory segment. */

int main(int argc, char* argv[])
{
    int shm_id;	      	      /* ID of the shared memory segment.   */
    char* shm_addr; 	      /* address of shared memory segment.  */
    int* countries_num;       /* number of countries in shared mem. */
    struct shmid_ds shm_desc;
    key_t set_key;
    
    set_key = ftok("/home/aravind/temp/shm", 'a');
    /* allocate a shared memory segment with size of 2048 bytes. */

    fprintf(stdout, "\nListener: set_key: %d", set_key);
    shm_id = shmget(set_key, 2048, IPC_CREAT | 0600);
    if (shm_id == -1) {
        perror("main: shmget: ");
        exit(1);
    }

    /* attach the shared memory segment to our process's address space. */
    shm_addr = shmat(shm_id, NULL, 0);
    if (!shm_addr) { /* operation failed. */
        perror("main: shmat: ");
        exit(1);
    }


    countries_num = (int*) shm_addr;
    fprintf(stdout, "Listener read %d. Changing it to 31 now", *countries_num);
    *countries_num = 31;

    fprintf(stdout, "Listener: Ara is now bow");


    if (shmdt(shm_addr) == -1) {
        perror("main: shmdt: ");
    }

    /* de-allocate the shared memory segment. */
    if (shmctl(shm_id, IPC_RMID, &shm_desc) == -1) {
        perror("main: shmctl: ");
    }

    return 0;
}
