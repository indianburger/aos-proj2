#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "core.h"



int main(int argc, char *argv[])
{
	int i;
	
	if(strcmp(argv[0], "s") == 0){ // synchronous
	
		printf(" This file will start accessing shared memory %s \n", argv[0]);
		for(i=0;i<atoi(argv[2])	;i++)
		{
			printf("parameter %d in the process with pid =  %d\n", i, atoi(argv[1]));
			// create packet 
			// put it in message queue using msgsend api
			// 
		}
	}
		
	return EXIT_SUCCESS;
}

void initPacket(){
	// create packet
	// give it some values
	
}
