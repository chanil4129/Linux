#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void)
{
	sigset_t sig_set;
	int count;

	sigemptyset(&sig_set);
	sigaddset(&sig_set, SIGINT);//시그널 추가
	sigprocmask(SIG_BLOCK, &sig_set, NULL);//시그널을 블록시킴

	//for문 실행동안 SIGINT의 발생이 있었다면, SIG_UNBLOCK 시그널을 sigprocmask()의 인수로 호출. 함수의 실행이 끝나고
	//SIGINT에 대한 디폴트 시그널 ㅍ핸들러가 실행. 만약 발생이 없었다면, 프로그램은 문자열을 출력하고 무한 루프에 빠짐.
	for (count = 3 ; 0 < count ; count--) {
		printf("count %d\n", count);
		sleep(1);
	}

	printf("Ctrl-C에 대한 블록을 해제\n");
	sigprocmask(SIG_UNBLOCK, &sig_set, NULL);//시그널 블록 해제
	printf("count중 Ctrl-C입력하면 이 문장은 출력 되지 않음.\n");

	while (1);

	exit(0);
}
