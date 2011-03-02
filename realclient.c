#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "core.h"

void initPacket(int i, int pid, diffie_pkt *pkt);

int main(int argc, char *argv[])
{
	int i;
	int num_of_requests = atoi(argv[2]);
	int synch_flag = -1;
	int ec;
	
	if(strcmp(argv[0], "s") == 0){ // synchronous
		synch_flag = 1;
		printf(" This file will start accessing shared memory %s \n", argv[0]);
		for(i=0;i<num_of_requests;i++)
		{
			printf("parameter %d in the process with pid =  %d\n", i, num_of_requests);
			// create packet 
			diffie_pkt *p = (diffie_pkt*)malloc(sizeof(diffie_pkt));
			initPacket(i, getpid(), p);
			// put it in message queue using put_pkt api
			ec = put_pkt(REQ_Q, *p);
			if(ec == -1){
				   fprintf(stderr, "\n unable to put_pkt for process %d", getpid());
				   return EXIT_FAILURE;
			}
			// now that i have put the packet in the queue i want to use it
			diffie_pkt result =  get_pkt(RESP_Q, getpid());
			printf(" This is the result received from the server %lld\n", result.out_result);
			
		}
	}
	/*if(strcmp(argv[0], "a") == 0){ // asynchronous
		synch_flag = 2;
		printf(" This file will start accessing shared memory %s \n", argv[0]);
		for(i=0;i<num_of_requests;i++)
		{
			printf("parameter %d in the process with pid =  %d\n", i, num_of_requests);
			// create packet 
			//diffie_pkt p = (diffie_pkt*)malloc(sizeof(diffie_pkt));
			initPacket(i, getpid(), &p);
			// put it in message queue using put_pkt
			ec = put_pkt(REQ_Q, p);
			if(ec == -1){
				   fprintf(stderr, "\n unable to put_pkt for process %d", getpid());
				   return EXIT_FAILURE;
			}
			if(i%10 == 0){
				// now that i have put the packet in the queue i want to use it
				//diffie_pkt result[MAX_RESULTS] =  = get_pkt_async(RESP_Q);
				//printf(" This is the result received from the server %lld\n", result->out_result);
			}
			
		}
	    }*/
		
	return EXIT_SUCCESS;
}

void initPacket(int i, int pid, diffie_pkt *pkt){
	// create packet
	// give it some values
	
	pkt->inp_x = i*3;
	pkt->inp_p = DUMMY_PVALUE;
	pkt->mtype = pid;
	pkt->req_id = i;	
	
//	return pkt;
	
}
