#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>

int ssu_daemon_init(void);

int main(void){
	printf("daemon process initialization\n");

	if(ssu_daemon_init()<0){
		fprintf(stderr,"ssu_daemon_init failed\n");
		exit(1);
	}

	//5초마다 로그 메세지를 남기게 하고 동작 확인. 에러도 확인 가능
	while(1){
		//라인 프린터 시스템. 각 메세지마다 프로세스 ID를 기록
		openlog("lpd",LOG_PID,LOG_LPR);
		syslog(LOG_ERR,"open failed lpd %m");
		closelog();
		sleep(5);
	}
	exit(0);
}

int ssu_daemon_init(void){
	pid_t pid;
	int fd, maxfd;
	
	//부모 프로세스 죽이고, 자식 프로세스를 쓴다
	if((pid=fork())<0){
		fprintf(stderr,"fork error\n");
		exit(1);
	}
	else if(pid!=0)
		exit(0);

	pid=getpid();
	printf("process %d running as daemon\n",pid);
	//독자적인 세션을 갖게함. 이렇게 하지 않으면 나중 프로세스가 부모 프로세션을 공유하게
	//되고, 부모가 종료하면 같이 종료되기 때문.
	setsid();
	//시그널을 무시하기 위함
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	maxfd=getdtablesize();

	//부모가 괜히 열어놓은 파일디스립터들이 있을 수 있기에 모든 fd를 닫음
	for(fd=0;fd<maxfd;fd++)
		close(fd);

	//파일 금지. 파일을 생성할 때 rwx가 막히지 않게함.
	umask(0);
	//작업 경로를 루트로 옮겨 작업을 편하게 할 수 있음
	chdir("/");
	//터미널을 없애는 효과
	fd=open("/dev/null",O_RDWR);
	dup(0);
	dup(0);
	return 0;
}
