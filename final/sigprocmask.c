#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
int main(void)
{
	sigset_t sig_set;

	sigemptyset(&sig_set);
	sigaddset(&sig_set,SIGINT);
	sigprocmask(SIG_BLOCK,&sig_set,NULL);

	for(int i=0;i<3;i++){
		printf("%d\n",i);
		sleep(1);
	}
	
	printf("ctrl-c block free\n");
	sigprocmask(SIG_UNBLOCK,&sig_set,NULL);
	printf("not seen if ctrl-c\n");

	while(1);
	
	exit(0);
}
