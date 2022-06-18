#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int ssu_daemon_init(void);

int main(void){
	pid_t pid;

	pid=getpid();
	printf("pid : %d\n",pid);
	printf("daemon init\n");
	if(ssu_daemon_init()<0){
		fprintf(stderr,"daemon error\n");
		exit(1);
	}
	exit(0);
}

int ssu_daemon_init(void){
	pid_t pid;
	int fd,maxfd;

	if((pid=fork())<0){
		fprintf(stderr,"fork error\n");
		exit(1);
	}
	else if(pid!=0)
		exit(0);

	pid=getpid();
	printf("pid : %d\n",pid);
	setsid();
	signal(SIGTSTP,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGTTOU,SIG_IGN);
	maxfd=getdtablesize();

	for(fd=0;fd<maxfd;fd++)
		close(fd);

	umask(0);
	chdir("/");
	fd=open("/dev/null",O_RDWR);
	dup(0);
	dup(0);
	return 0;
}
