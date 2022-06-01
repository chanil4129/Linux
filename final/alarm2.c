#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define LINE_MAX 2048

static void ssu_alarm(int signo);

int main(void){
	char buf[LINE_MAX];
	int n;
	if(signal(SIGALRM,ssu_alarm)==SIG_ERR){
		fprintf(stderr,"error\n");
		exit(EXIT_FAILURE);
	}
	alarm(10);
	if((n=read(STDIN_FILENO,buf,LINE_MAX))<0){
		fprintf(stderr,"error\n");
		exit(EXIT_FAILURE);
	}
	alarm(0);
	write(STDOUT_FILENO,buf,n);
	exit(0);
}

static void ssu_alarm(int signo) {
	printf("SIGALRM\n");
}
