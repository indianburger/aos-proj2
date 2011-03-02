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
}

