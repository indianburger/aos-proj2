#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

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


int put_pkt(int key_id, diffie_pkt pkt);
int get_q_id(int key_id);
int get_q_id_gen(int key_id, int flag);
int get_key_id(int key_id);
diffie_pkt get_pkt(int key_id);
void initialize();
void clean_up();
/* QUEUE FUNCTIONS */
int put_pkt(int key_id, diffie_pkt pkt){
    int msqid = get_q_id(key_id); 

    if (msgsnd(msqid, &pkt, sizeof(diffie_pkt), IPC_NOWAIT) < 0) {
        fprintf(stderr, "\nput_pkt error for key_id: %d", key_id);
        return -1; 
    }
    return 0;
}

int get_q_id(int key_id){
    int x = get_q_id_gen(key_id, -1);
    fprintf(stderr, "\nget_q_id: key id %d for %d", x, key_id);
    return x;
}

int get_q_id_gen(int key_id, int flag){
    int msqid;
    int msgflg;
    if (flag == -1) msgflg = 0666;
    else msgflg = flag | 0666;
    key_t key = get_key_id(key_id);
    
    if ((msqid = msgget(key, msgflg )) < 0) {
        fprintf(stderr, "\nmsgget error for %d", key_id);
    }
    return msqid;
}

int get_key_id(int key_id){
    int x = ftok(key_id == REQ_Q ? REQ_KEY_FILE: RESP_KEY_FILE, 1);
    fprintf(stderr, "\nget_key_id: key id %d for %d", x, key_id);
    return x;
}

diffie_pkt get_pkt(int key_id){
    int msqid = get_q_id(key_id); 
    diffie_pkt pkt;
    if (msgrcv(msqid, &pkt, sizeof(diffie_pkt), 0, 0) < 0) {
        fprintf(stderr, "\nmsgget error for %d", key_id);
    }
    else
        pkt.out_result = pkt.inp_p * pkt.inp_x;
    //printf("Received this: %ld %d %d\n", pkt.mtype, pkt.inp_p, pkt.inp_x);
    //put_pkt(RESP_KEY_ID, pkt);
    return pkt;
}

/* SEMAPHORE FUNCTIONS */

/* GLOBAL INITIALIZE AND CLEANUP */
void initialize(){
    get_q_id_gen(REQ_Q, IPC_CREAT);
    get_q_id_gen(RESP_Q, IPC_CREAT);
}


void clean_up(){
    int msqid;


    fprintf(stderr, "\nclean_up: req");
    msqid = get_q_id(REQ_Q);

    if (msgctl(msqid, IPC_RMID, NULL) < 0)
        fprintf(stderr, "\n\nError clearing queue %d and msqid %d", REQ_Q, msqid);
    

    fprintf(stderr, "\nclean_up: resp");
    msqid = get_q_id(RESP_Q);
    if (msgctl(msqid, IPC_RMID, NULL) < 0)
        fprintf(stderr, "\n\nError clearing queue %d and msqid %d", RESP_Q, msqid);
}

