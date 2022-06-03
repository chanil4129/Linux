#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <unistd.h>

void chan(int signum){
	printf("signal\n");
}

int main(void){
	printf("start\n");
	if(signal(SIGALRM,chan)==SIG_ERR){
		fprintf(stderr,"error\n");
		exit(EXIT_FAILURE);
	}
	alarm(10);
	pause();
	printf("end\n");
	exit(0);
}

