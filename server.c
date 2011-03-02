#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<time.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<sys/ipc.h>
#include<sys/signal.h>
#include "core.h"
//int createSharedMemory();


long long int computeDH(int x, long long int p);

void SignalHandler(int sig){
	//fprintf(stderr, "server signal handler: - %d", getpid());
	switch (sig){
		case SIGUSR1: 
			fprintf(stderr, "\nserver: quitting. Cleaning up\n");
			// do memory deallocation here 
			clean_up();
			exit(0);
			break;
		}
}

int main(int argc, char* argv[])
{
	int child_pid;
	struct timespec delay;
	int i;
	int pid;
	int ec;
	
	signal(SIGUSR1,SignalHandler); /* catch signal */
	if(argc > 1){
		pid = atoi(argv[2]);
		printf("%d", pid);
		if(strcmp(argv[1], "stop") == 0)
			kill(pid, SIGUSR1);	
		exit(0);
	}
	
	//printf("i should not be printed %d\n", getpid());
	child_pid = fork();

	switch(child_pid){
		case -1:
			perror("fork");
		    exit(1);
		case 0: // child process is starting here
			fprintf(stderr, "the child process has now started\n %d", getpid());
			// do shared memory allocation before the while 
			// -1 then error
			createSharedMemory();
			while(1){
				
					diffie_pkt pkt = get_pkt_async(REQ_Q, 0); // get_pkt server sends 0
                    if (pkt.mtype != -1){
					    //fprintf(stderr, "\nserver: Got packed. Let's see what I can do");
					    pkt.out_result = computeDH(pkt.inp_x, pkt.inp_p);
					    //fprintf(stderr, "\nserver: computed. Now going to put it back.");
					    ec = put_pkt(RESP_Q, pkt);
					    if(ec == -1){
					    	fprintf(stderr, "\n unable to put_pkt for in server code");
					        return EXIT_FAILURE;
					    }
                    }
                    else{
                        sleep(1);
                    }
				}
			break;

		default:
			//printf("in parent - will now exit\n");
			exit(0);
	}			
}

int createSharedMemory(){
	initialize();
	//printf("in create shared memory \n");
	return 0;
}

long long int computeDH(int x, long long int p){
	
	int i=0;
	long long int num = 1;
	long long int result;
	for(i=0;i<x;i++){
		num = num * 2;
	}
	result = num%p;
	fprintf(stderr, "\nserver computeDH:x: %d p: %lld num: %lld  result: %lld", x, p, num, result);
	return result;
}


