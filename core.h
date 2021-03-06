#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#define REQ_KEY_FILE "req_key_file" 
#define RESP_KEY_FILE "resp_key_file" 
#define SEM_KEY_FILE "sem_key_file" 
#define REQ_Q 0
#define RESP_Q 1
#define KEY_ID 1

#define DUMMY_PVALUE 19

typedef struct diffie_pkt {
    long mtype;
    int req_id;
    int inp_x;
    long long int inp_p;
    long long int out_result;
} diffie_pkt;


int put_pkt(int queue_type, diffie_pkt pkt);
int get_q_id(int queue_type);
int get_q_id_gen(int queue_type, int flag);
int get_q_key_id(int queue_type);
diffie_pkt get_pkt(int queue_type, int pid);
void initialize();
void clean_up();
/* QUEUE FUNCTIONS */
int put_pkt(int queue_type, diffie_pkt pkt){
    int msqid = get_q_id(queue_type); 
    //fprintf(stderr, "\nput_pkt: locking");
    sem_lock(queue_type);
    if (msgsnd(msqid, &pkt, sizeof(diffie_pkt), IPC_NOWAIT) < 0) {
        //fprintf(stderr, "\nput_pkt error for queue_type: %d", queue_type);
        //fprintf(stderr, "\nput_pkt: unlocking");
        sem_unlock(queue_type);
        return -1; 
    }
    //fprintf(stderr, "\nput_pkt: unlocking");
    sem_unlock(queue_type);
    return 0;
}

int get_q_id(int queue_type){
    int x = get_q_id_gen(queue_type, -1);
    //fprintf(stderr, "\nget_q_id: key id %d for %d", x, queue_type);
    return x;
}

int get_q_id_gen(int queue_type, int flag){
    int msqid;
    int msgflg;
    if (flag == -1) msgflg = 0666;
    else msgflg = flag | 0666;
    key_t key = get_q_key_id(queue_type);
    
    if ((msqid = msgget(key, msgflg )) < 0) {
        fprintf(stderr, "\nmsgget error for %d", queue_type);
    }
    return msqid;
}

int get_q_key_id(int queue_type){
    int x = ftok(queue_type == REQ_Q ? REQ_KEY_FILE: RESP_KEY_FILE, 1);
    //fprintf(stderr, "\nget_q_key_id: key id %d for %d", x, queue_type);
    return x;
}

/* pid=0 gives first packet in queue. Else it will
 * look in queue for mtype = pid*/
diffie_pkt get_pkt(int queue_type, int pid){
    int msqid = get_q_id(queue_type); 
    diffie_pkt pkt;
    while(1){
        //fprintf(stderr, "\nget_pkt: locking");
        sem_lock(queue_type);
        //fprintf(stderr, "\nget_pkt: Got lock");
        if (msgrcv(msqid, &pkt, sizeof(diffie_pkt), pid, IPC_NOWAIT) < 0) {
            //fprintf(stderr, "\nget_pkt %d: no msg, unlocking, sleeping and trying again", queue_type);
            sem_unlock(queue_type);
            sleep(1);
        }
        else{

            break;
        }
    }
    //printf("Received this: %ld %d %d\n", pkt.mtype, pkt.inp_p, pkt.inp_x);
    //put_pkt(RESP_KEY_ID,t pkt);
    //fprintf(stderr, "\nget_pkt: unlocking");
    sem_unlock(queue_type);
    return pkt;
}
diffie_pkt get_pkt_async(int queue_type, int pid){
    int msqid = get_q_id(queue_type); 
    diffie_pkt pkt;
    //fprintf(stderr, "\nget_pkt_async: locking");
    sem_lock(queue_type);
    //fprintf(stderr, "\nget_pkt_asyn: Got lock");
    if (msgrcv(msqid, &pkt, sizeof(diffie_pkt), pid, IPC_NOWAIT) == -1) {
        //fprintf(stderr, "\nget_pkt_async: no packets left for queue type %d", queue_type);
        pkt.mtype = -1;
    }
    //printf("Received this: %ld %d %d\n", pkt.mtype, pkt.inp_p, pkt.inp_x);
    //put_pkt(RESP_KEY_ID,t pkt);
    //fprintf(stderr, "\nget_pkt: unlocking");
    sem_unlock(queue_type);
    return pkt;
}

/* SEMAPHORE FUNCTIONS */
int get_sem_key_id(){
    int x = ftok(SEM_KEY_FILE, 1);
    //fprintf(stderr, "\nget_sem_key_id: key id %d ", x);
    return x;
}

int get_sem_id(flag){
    int semid;
    int msgflg;
    if (flag == -1) msgflg = 0666;
    else msgflg = flag | 0666;
    key_t key = get_sem_key_id();
    
    if ((semid = semget(key, 2, msgflg )) < 0) {
        fprintf(stderr, "\nsemget error");
    }
    return semid;
}
/* lockid represents request of response. 
 * It can take values REQ_SEM or RESP_SEM*/
int sem_lock(int queue_type){
    struct sembuf sem_op; 
    sem_op.sem_num = queue_type;
	sem_op.sem_op = -1;
	sem_op.sem_flg = 0;
	semop(get_sem_id(-1), &sem_op, 1);

}

int sem_unlock(int queue_type){
    struct sembuf sem_op; 
	sem_op.sem_num = queue_type;
	sem_op.sem_op = 1;
	sem_op.sem_flg = 0;
	semop(get_sem_id(-1), &sem_op, 1);
}

/* GLOBAL INITIALIZE AND CLEANUP */
void initialize(){

    int semid = get_sem_id(IPC_CREAT);
    
    //fprintf(stderr, "\n initialize: just finished semid");
    semctl(semid, REQ_Q, SETVAL, 1); //initialize semaphore to 1
    semctl(semid, RESP_Q, SETVAL, 1); //initialize semaphore to 1
    //fprintf(stderr, "\nDraining start for req q");
    get_q_id_gen(REQ_Q, IPC_CREAT);
    //fprintf(stderr, "\nGot queueid");
    while(1){
        diffie_pkt pkt = get_pkt_async(REQ_Q, 0);
        if (pkt.mtype == -1) break;
        else fprintf(stderr, "\nDrained pkt from req q");
    }
    //fprintf(stderr, "\nDraining start for resp q");
    get_q_id_gen(RESP_Q, IPC_CREAT);
    while(1){
        diffie_pkt pkt = get_pkt_async(RESP_Q, 0);
        if (pkt.mtype == -1) break;
        else fprintf(stderr, "\nDrained pkt from req q");
    }

}


void clean_up(){
    int msqid;

    /* QUEUE CLEANUP */
    //fprintf(stderr, "\nclean_up: req");
    msqid = get_q_id(REQ_Q);

    if (msgctl(msqid, IPC_RMID, NULL) < 0)
        fprintf(stderr, "\n\nError clearing queue %d and msqid %d", REQ_Q, msqid);
    

    //fprintf(stderr, "\nclean_up: resp");
    msqid = get_q_id(RESP_Q);
    if (msgctl(msqid, IPC_RMID, NULL) < 0)
        fprintf(stderr, "\n\nError clearing queue %d and msqid %d", RESP_Q, msqid);

    /* SEMAPHORE CLEANUP */

    if (semctl(get_sem_id(-1), 0, IPC_RMID) < 0)
        fprintf(stderr, "\n\nclean_up: Error clearing semaphore");
}

