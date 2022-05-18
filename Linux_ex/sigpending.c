#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <signal.h> 
int main(void) 
{ 
	sigset_t pendingset; 
	sigset_t sig_set; 
	int count = 0; 
	sigfillset(&sig_set);  // 모든 시그널을 sig_set에 추 
	sigprocmask(SIG_SETMASK, &sig_set, NULL); //모든 시그널 블
	while (1) { 
		printf("count: %d\n", count++); 
		sleep(1); 
		if (sigpending(&pendingset) == 0) { //팬딩중인 시그널 집합 탐색
			if (sigismember(&pendingset, SIGINT)) {  // SIGINT 시그널이 포함되어 있는지 검
				printf("SIGINT가 블록되어 대기 중. 무한 루프를 종료.\n"); break; 
			} 
		} 
	} 
	exit(0); 
} 

