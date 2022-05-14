#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <time.h>

static void ssu_alarm(int signo);
static void ssu_func(int signo);
void ssu_mask(const char *str);

static volatile sig_atomic_t can_jump;
static sigjmp_buf jump_buf;

int main(void){
	//SIGUSR1 시그널 ssu_func로 핸들러 설정
	if(signal(SIGUSR1, ssu_func)==SIG_ERR){
		fprintf(stderr,"SIGUSR1 error");
		exit(1);
	}

	//SIGALRM 시그널 ssu_alarm로 핸들러 설정
	if(signal(SIGALRM,ssu_alarm)==SIG_ERR){
		fprintf(stderr,"SIGALRM error");
		exit(1);
	}

	ssu_mask("starting main: ");

	//siglongjmp의 호출이 오면 종료
	if(sigsetjmp(jump_buf,1)){
		ssu_mask("ending main: ");
		exit(0);
	}

	can_jump=1;
	
	//시그널 올때까지 계속 일시정지 무한반복
	while(1)
		pause();

	exit(0);
}

void ssu_mask(const char *str){
	sigset_t sig_set;
	int err_num;

	err_num=errno;

	//sig_set은 블럭되는 현재 시그널 집합을 반환
	if(sigprocmask(0,NULL,&sig_set)<0){
		printf("sigprocmask() error");
		exit(1);
	}

	printf("%s",str);

	//각 시그널 집합의 멤버를 확인하여 있다면 해당 시그널 출력
	if(sigismember(&sig_set,SIGINT))
		printf("SIGINT ");

	if(sigismember(&sig_set,SIGQUIT))
		printf("SIGQUIT ");

	if(sigismember(&sig_set,SIGUSR1))
		printf("SIGUSR1 ");

	if(sigismember(&sig_set,SIGALRM))
		printf("SIGALRM ");

	printf("\n");
	errno=err_num;
}

//사용자 정의 핸들러. 사용자 명령어 kill로 실행
static void ssu_func(int signo){
	time_t start_time;

	if(can_jump==0)
		return;

	ssu_mask("starting ssu_func: ");
	alarm(3);//3초 alarm
	start_time=time(NULL);

	//총 5초가 지나면 break
	while(1)
		if(time(NULL)>start_time+5)
			break;

	//종료
	ssu_mask("ending ssu_func: ");
	can_jump=0;
	//sigsetjump 호출
	siglongjmp(jump_buf,1);
}

static void ssu_alarm(int signo){
	ssu_mask("in ssu_alarm: ");
}

