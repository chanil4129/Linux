#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static void sig_alarm(int signo){
	printf("signo:%d\n",signo);
}

unsigned int ssu_sleep(unsigned int seconds){
	int remain;
	remain=alarm(seconds);
	if(remain>0)
		sig_alarm(SIGALRM);
	pause();
}

int main(int argc,char * argv[]){
	unsigned int t;

	if(argc>2){
		fprintf(stderr,"usage : %s <time>\n",argv[0]);
		exit(1);
	}
	signal(SIGALRM,sig_alarm);
	printf("3 seconds sleeping\n");
	alarm(3);
	if(argc!=1){
		t=atoi(argv[1]);
		ssu_sleep(t);
		printf("sleeping time is changed to %d\n",t);
	}
	
	exit(0);
}
