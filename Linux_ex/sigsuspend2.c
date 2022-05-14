#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

static void ssu_func(int signo);
void ssu_print_mask(const char *str);

int main(void){
	sigset_t new_mask, old_mask, wait_mask;

	ssu_print_mask("program start: ");
	//SIGINT 핸들러 ssu_func로 설정
	if(signal(SIGINT,ssu_func)==SIG_ERR){
		fprintf(stderr,"signal(SIGINT) error\n");
		exit(1);
	}

	//wait_mask 시그널 집합들 초기화
	sigemptyset(&wait_mask);
	//wait_mask에 SIGUSR1 마스킹 추가
	sigaddset(&wait_mask,SIGUSR1);
	//new_mask 시그널 집합들 초기화
	sigemptyset(&new_mask);
	//new_mask에 SIGINT 마스킹 추가
	sigaddset(&new_mask,SIGINT);

	//new_mask 마킹 돼있는 SIGINT 블럭하고, old_mask에 집합 복제
	if(sigprocmask(SIG_BLOCK,&new_mask,&old_mask)<0){
		fprintf(stderr,"SIG_BLOCK() error\n");
		exit(1);
	}

	ssu_print_mask("in critical region: ");

	//wait_mask의 SIGUSR1 시그널이 발생할 때까지 잠시 팬딩
	if(sigsuspend(&wait_mask)!=-1){
		fprintf(stderr,"sigsuspend() error\n");
		exit(1);
	}

	ssu_print_mask("after return from sigsuspend: ");

	//old_mask 마킹 돼있는 SIGINT 블럭해제
	if(sigprocmask(SIG_SETMASK, &old_mask,NULL)<0){
		fprintf(stderr,"SIG_SETMASK() error\n");
		exit(1);
	}

	ssu_print_mask("program exit: ");
	exit(0);
}

void ssu_print_mask(const char *str){
	sigset_t sig_set;
	int err_num;

	err_num=errno;

	//sig_set은 시그널 이전 집합이 됨
	if(sigprocmask(0,NULL,&sig_set)<0){
		fprintf(stderr,"sigprocmask() error\n");
		exit(1);
	}

	printf("%s", str);

	//sig_set 집합에 해당 멤버가 마스킹 돼있으면 출력
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

static void ssu_func(int signo){
	ssu_print_mask("\nin ssu_func: ");
}
