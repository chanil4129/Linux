#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int main(void){
	int pid;

	printf("pid : %d\n",getpid());
	
	scanf("%d",&pid);
	if(getpid()==pid)
		kill(pid,SIGKILL);

	
	while(1)
		pause();

	exit(0);
}
