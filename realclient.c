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
	int async_pkt_ctr;
	
	if(strcmp(argv[0], "s") == 0){ // synchronous
		synch_flag = 1;
		fprintf(stderr, " This file will start accessing shared memory %s \n", argv[0]);
		for(i=0;i<num_of_requests;i++)
		{
			diffie_pkt p ;
			fprintf(stderr,"process with pid =  %d , number of requests = %d, and this is request # = %d\n", getpid(), num_of_requests, i);
			// create packet 
			//diffie_pkt p = (diffie_pkt*)malloc(sizeof(diffie_pkt));
			initPacket(i, getpid(), &p);
			
			// put it in message queue using put_pkt api
			ec = put_pkt(REQ_Q, p);
			if(ec == -1){
				   fprintf(stderr, "\n unable to put_pkt sync for process %d", getpid());
				   return EXIT_FAILURE;
			}
			// now that i have put the packet in the queue i want to use it
			diffie_pkt result =  get_pkt(RESP_Q, getpid());
			fprintf(stderr," This is the sync result received from the server %lld\n", result.out_result);
			
		}
	}
	if(strcmp(argv[0], "a") == 0){ // asynchronous
		synch_flag = 2;
		async_pkt_ctr = num_of_requests;
		diffie_pkt result;
		fprintf(stderr, "\n This file will start accessing shared memory %s \n", argv[0]);
		
		for(i=0;i<num_of_requests;i++)
		{
			diffie_pkt p ;
			fprintf(stderr, "\nprocess with pid =  %d , number of requests = %d, and this is request # = %d\n", getpid(), num_of_requests, i);
			
			// create packet 
			//diffie_pkt p = (diffie_pkt*)malloc(sizeof(diffie_pkt));
			initPacket(i, getpid(), &p);
			
			// put it in message queue using put_pkt
			ec = put_pkt(REQ_Q, p);			
			if(ec == -1){
				   fprintf(stderr, "\n unable to put_pkt in async for process %d", getpid());
				   return EXIT_FAILURE;
			}
			
			if(i%10 == 0){
				// now that i have put the packet in the queue i want to use it
				do{
					fprintf(stderr,"\nrealclient: inside 10");
					result = get_pkt_async(RESP_Q, getpid());
					
					if(result.mtype != -1){
						async_pkt_ctr--;
						fprintf(stderr,"\nNOT DRAINED realclient:result received at realclient pid =%d from the server %lld\n", getpid(), result.out_result);
					}
					
				}while(result.mtype != -1);

			}
            else{
                //sleep(1);
                usleep(1000 * 100);   
            }
			
		}
		if(async_pkt_ctr != 0){
			// do the draining here 
			do{
					result = get_pkt_async(RESP_Q, getpid());
					if(result.mtype != -1){
						async_pkt_ctr--;
						fprintf(stderr,"\nDRAINING: result received at realclient pid =%d from the server %lld\n", getpid(), result.out_result);
					}
					
				}while(async_pkt_ctr != 0);
		}
   }
		
	return EXIT_SUCCESS;
}

void initPacket(int i, int pid, diffie_pkt *pkt){
	// create packet
	// give it some values
	
	pkt->inp_x = i*3 % 63;
	pkt->inp_p = DUMMY_PVALUE;
	pkt->mtype = pid;
	pkt->req_id = i;	
	
//	return pkt;
	
}
