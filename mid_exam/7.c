#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/wait.h>

double ssu_maketime(struct timeval *time);

void term_stat(int stat);

void ssu_print_child_info(int stat, struct rusage *rusage);

int main(void){
	struct rusage rusage;
	pid_t pid;
	int status;
	
	if((pid=fork())<0){
		fprintf(stderr,"fork error\n");
		exit(1);
	}

	else if(pid==0){
		char *argv[]={"/usr/bin/find","find",(char *)0};
		execv("/usr/bin/find",argv);
	}

	if(wait3(&status,1,&rusage)<0){
	}
		




	exit(0);
}
