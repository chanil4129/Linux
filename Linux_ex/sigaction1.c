#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_signal_handler(int signo) {
	printf("ssu_signal_handler control\n");
}

int main(void) {
	struct sigaction sig_act;
	sigset_t sig_set;

	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = 0;
	sig_act.sa_handler = ssu_signal_handler;
	sigaction(SIGUSR1, &sig_act, NULL);//SIGUSR1 시그널 액션 변경 확인
	printf("before first kill()\n");
	kill(getpid(), SIGUSR1);
	sigemptyset(&sig_set);//sigset 초기화
	sigaddset(&sig_set, SIGUSR1);
	sigprocmask(SIG_SETMASK, &sig_set, NULL);//SIGUSR1을 블록
	printf("before second kill()\n");
	kill(getpid(), SIGUSR1);//현재 프로세스에 SIGUSR1 시그널을 발생
	printf("after second kill()\n");
	exit(0);
}
