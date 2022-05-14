#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

void ssu_signal_handler(int signo);

jmp_buf jump_buffer;

int main(void) {
	//Intterupt 핸들러 추가
	signal(SIGINT, ssu_signal_handler);

	//그냥 호출 됐기 때문 출력하고나서 시그널 받을 때까지 일시 정지
	while (1) {
		if (setjmp(jump_buffer) == 0) {
			printf("Hit Ctrl-c at anytime ...\n");
			pause();
		}
	}
	exit(0);
}

void ssu_signal_handler(int signo) {
	char character;

	//SIG_IGN 액션 추가
	signal(signo, SIG_IGN);
	printf("Did you hit Ctrl-c?\n" "Do you really want to quit? [y/n] ");
	//사용자로부터 입력받기. y,Y라면 종료
	character = getchar();
	if (character == 'y' || character == 'Y')
		exit(0);
	//아니라면 핸들러 다시 설정하 longjmp() 호출. 이전 setjmp 위치로 점프
	//ssu_singnal)handler는 리턴하지 않은 상태이기 때문에 SIGINT는 블록된 상태
	//따라서 SIGINT를 발생시켜도 블록돼서 핸들러가 실행하 않는다.
	else {
		signal(SIGINT, ssu_signal_handler);
		longjmp(jump_buffer, 1);
	}
}
