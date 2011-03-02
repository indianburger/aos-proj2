#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<time.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<sys/ipc.h>
#include<sys/signal.h>

//int createSharedMemory();
//long int computeDH(int x, long int p);

void SignalHandler(int sig){
	printf("in signal handler - %d", getpid());
	switch (sig){
		case SIGUSR1: 
			printf("SIGUSR1 Caught - need to deallocate memory now.... \n");
			// do memory deallocation here 
			
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
	
	printf("i should not be printed %d\n", getpid());
	child_pid = fork();

	switch(child_pid){
		case -1:
			perror("fork");
		    exit(1);
		case 0: // child process is starting here
			printf("the child process has now started\n %d", getpid());
			fflush(stdout);
			// do shared memory allocation before the while 
			// -1 then error
			//ec = createSharedMemory();
			//if(ec != -1) // after creating shared memory stay in the while(true) loop and keep checking if the queues have anything to be computed
				while(1){
				
					//for all queues do this 
						//if queue has anything to compute
						{
							// computeDH
							// writeResultToQueue
						}
						//move to the next queue
				}
	
			break;

		default:
			//computeDH(62,9369319);
			
			printf("in parent - will now exit\n");
			exit(0);
	}			
}
/*
int createSharedMemory(){
	printf("in create shared memory \n");
	return 0;

}

long int computeDH(int x, long int p){
	
	int i=0;
	long long int num = 1;
	long long int result;
	for(i=0;i<x;i++){
		num = num * 2;
	}
	result = num%p;
	printf("%lld  %lld", num, result);
	return result;
}

// compute DH and then write it to the queue.. 

int writeResultToQueue(){
	
	int msqid; // id of the queue
	struct msgbuf* msg;
	int msgsz; // size of the data part in bytes
	int msgflag = IPC_NOWAIT;
	
	// semaphore for obtaining the response queue 
	int sem_set_id; // id of the semaphore set
	int sem_val; // the initial semaphore value
	int semaphoreId;
	
	sem_set_id = semget(semaphoreId, 1, IPC_CREAT | 0600)
	if(sem_set_id == -1){
		perror("error in semget");
		return -1;
	}
	printf("semaphore created\n");
	sem_val = 0;
		
	int errorcode = msgsnd(queue_id, msg, msgsz, msgflag);
	if(errorcode == -1){
		perror("msgsnd");
		return -1;
	}
	return 0;
}
	
	
}
*/

