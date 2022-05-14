#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void){
	sigset_t old_set;
	sigset_t sig_set;

	//sig_set 집합의 마스크를 비움.
	sigemptyset(&sig_set);
	//SIGINT 마스킹을 추가
	sigaddset(&sig_set, SIGINT);
	//마스킹된거 블록 시키 old_set에 시그널 집합 저장
	sigprocmask(SIG_BLOCK, &sig_set, &old_set);
	//old_set에 있는 시그널인 SIGINT가 발생할 때까지 프로세스는 일시정지
	sigsuspend(&old_set);
	exit(0);
}
