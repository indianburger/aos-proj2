#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "include.h"
int send_request(int pid, int x, int p);  
int rcv_response();
int main ( int argc, char *argv[] )
{
    send_request(getpid(), 1, 2);
    rcv_response();
    return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */

int send_request(int pid, int x, int p){
    int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key = ftok(REQ_Q_KEY, 'a');
    diffie_pkt pkt;
    
    if ((msqid = msgget(key, msgflg )) < 0) {
        perror("msgget");
        return -1;
    }
    else 
     fprintf(stderr,"msgget: msgget succeeded: msqid = %d\n", msqid);
    
    
    pkt.mtype = 1; 
    pkt.inp_x = x;
    pkt.inp_p = p;

    if (msgsnd(msqid, &pkt, sizeof(diffie_pkt), IPC_NOWAIT) < 0) {
        printf ("msqid: %d, type: %ld, x: %d, p: %d\n", msqid, pkt.mtype, pkt.inp_x, pkt.inp_p);
        perror("msgsnd");
        exit(1);
    }
    
    return 0;
}
      
int rcv_response(){
    int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key = ftok(REQ_Q_KEY, 'a');
    diffie_pkt pkt;
    
    if ((msqid = msgget(key, msgflg )) < 0) {
        perror("msgget");
        return -1;
    }
    else 
     fprintf(stderr,"msgget: msgget succeeded: msqid = %d\n", msqid);
    if (msgrcv(msqid, &pkt, sizeof(diffie_pkt), 1, 0) < 0) {
        perror("msgrcv");
        exit(1);
    }
    printf("Received this: %ld %d %d\n", pkt.mtype, pkt.inp_p, pkt.inp_x);
    return 1;
}

