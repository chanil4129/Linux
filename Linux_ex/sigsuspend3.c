#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

void ssu_signal_handler(int signo);
void ssu_timestamp(char *str);

int main(void) {
	struct sigaction sig_act;
	sigset_t blk_set;

	//blk_set 집합 전부 마스킹
	sigfillset(&blk_set);
	//blk_set에서 SIGALRM 마스킹 해제
	sigdelset(&blk_set, SIGALRM);
	//sigaction 하기전 초기화.
	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = 0;
	sig_act.sa_handler = ssu_signal_handler;
	//sigaction으로 시그널 처리 설정
	sigaction(SIGALRM, &sig_act, NULL);
	ssu_timestamp("before sigsuspend()");
	//5초 alarm
	alarm(5);
	//blk_set이 가지고 있는 집합의 시그널이 발생할 때가지 팬딩하다가
	//SIGALRM 시그널이 발생하면 등록된 시그널 핸들러 실행
	sigsuspend(&blk_set);
	ssu_timestamp("after sigsuspend()");
	exit(0);
}

void ssu_signal_handler(int signo) {
	printf("in ssu_signal_handler() function\n");
}

//시간 출력
void ssu_timestamp(char *str) {
	time_t time_val;

	time(&time_val);
	printf("%s the time is %s\n", str, ctime(&time_val));
}
