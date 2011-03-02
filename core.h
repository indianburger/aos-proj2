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
typedef struct diffie_pkt {
    long mtype;
    int req_id;
    int inp_x;
    int inp_p;
    int out_result;
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
    
    sem_lock(queue_type);
    if (msgsnd(msqid, &pkt, sizeof(diffie_pkt), IPC_NOWAIT) < 0) {
        fprintf(stderr, "\nput_pkt error for queue_type: %d", queue_type);
        sem_unlock(queue_type);
        return -1; 
    }
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
    sem_lock(queue_type);
    if (msgrcv(msqid, &pkt, sizeof(diffie_pkt), pid, 0) < 0) {
        fprintf(stderr, "\nmsgget error for %d", queue_type);
        sem_unlock(queue_type);
        //TODO: return with some sort of error indication
        //null can't be set I think
    }
    else
        pkt.out_result = pkt.inp_p * pkt.inp_x;
    //printf("Received this: %ld %d %d\n", pkt.mtype, pkt.inp_p, pkt.inp_x);
    //put_pkt(RESP_KEY_ID, pkt);
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
    semctl(semid, REQ_Q, SETVAL, 0); //initialize semaphore to 0
    semctl(semid, RESP_Q, SETVAL, 0); //initialize semaphore to 0

    get_q_id_gen(REQ_Q, IPC_CREAT);
    get_q_id_gen(RESP_Q, IPC_CREAT);

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
    
}

