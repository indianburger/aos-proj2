#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/bn.h>
#include <math.h>
#include "core.h"

void initPacket(int i, int pid, diffie_pkt *pkt);
long long int generatePrime(int d);

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
			fprintf(stderr,"\n Sending packet for process with pid =  %d , number of requests = %d, and this is request # = %d\n", getpid(), num_of_requests, i);
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
			fprintf(stderr,"\n Sync result received from the server for process with pid = %d request #= %d  is %lld\n", getpid(), result.req_id, result.out_result);
			
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
			fprintf(stderr, "\n Sending packet for process with pid =  %d , number of requests = %d, and this is request # = %d\n", getpid(), num_of_requests, i);
			
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
					//fprintf(stderr,"\nrealclient: inside 10");
					result = get_pkt_async(RESP_Q, getpid());
					
					if(result.mtype != -1){
						async_pkt_ctr--;
						fprintf(stderr,"\n Async result received from the server for process with pid = %d request #= %d  is %lld\n", getpid(), result.req_id, result.out_result);
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
						fprintf(stderr,"\nDRAINING: result received for pid =%d request # = %d from the server is %lld\n", getpid(), result.req_id, result.out_result);
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
	pkt->inp_p = generatePrime(50);//DUMMY_PVALUE;
	pkt->mtype = pid;
	pkt->req_id = i;	
	
//	return pkt;	
}

void success(){}
long long int generatePrime(int dummy){
	
	char *prime;
	BIGNUM *num_tmp;
	long int num_bits = 50; // 50 bit large prime number will be generated
	int i;
	int base=10;
	long long int num = 0;
	int len;
	
	if(dummy==0)
		return DUMMY_PVALUE;	
	
	num_tmp = BN_new();
	BN_generate_prime(num_tmp, num_bits,1,NULL,NULL,success,NULL);
	prime = (char*)malloc(BN_num_bytes(num_tmp));
	prime = BN_bn2dec(num_tmp);
		
	len = strlen(prime)-1;
	
	for(i = 0; i < strlen(prime); i++){
		//printf("num = %lld\n", num);
		num = num+ pow(base,(len-i))*(prime[i]-'0');
	}
	//printf("in generate prime - after conversion prime generated  = %lld \n",num );
	return num;
	
	
}


