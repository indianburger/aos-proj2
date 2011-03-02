#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define EXEC_PATH "realclient"
//#include "core.h"
//int send_request(int pid, int x, int p);  
//int rcv_response();
int main ( int argc, char *argv[] )
{

	int child_pid;
	int i;
	int k=0;
	char *str;	
	
	if(argc < 3){
		printf("Too few arguments\n");
		return EXIT_FAILURE;
	}

	if(strcmp(argv[1], "a") != 0 && strcmp(argv[1], "s") != 0){
		printf(" first parameter needs to be sync or async\n");
		return EXIT_FAILURE;
	}
	
	if(atoi(argv[2]) != (argc-3)){	
		printf(" Mismatched number of arguments, have to pass requests to all processes\n");
		return EXIT_FAILURE;
	}
		
	
	for(i=0;i<atoi(argv[2]);i++){
		child_pid = fork();
		switch(child_pid){
		
		case -1:
				perror("fork");
				exit(1);
		case 0: // child process is starting here
				printf(" in child process number %d  %d passed arguments %d\n", i, getpid(), atoi(argv[3+i]));
				// execute some process here - the real client 		

				sprintf(str, "%d", getpid());
				char *args[] = {argv[1], str , argv[3+i], (char *) 0 };
				printf("%d\n", execvp(EXEC_PATH, args));
				i++;
				printf(" executed");
				//exit(0);
				return EXIT_SUCCESS;
		default:
				printf("in parent - %d\n", getpid());
				//child_pid = fork();
		}
	}
	
	printf("My Pid = %d\n", getpid());
    return EXIT_SUCCESS;
}

