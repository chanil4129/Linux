#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>
#include <signal.h>

static void ssu_signal_handler1(int signo);
static void ssu_signal_handler2(int signo);

sigjmp_buf jmp_buf1;
sigjmp_buf jmp_buf2;

int main(void){
	struct sigaction act_sig1;
	struct sigaction act_sig2;
	int i,ret;

	printf("My PID is %d\n",getpid());
	ret=sigsetjmp(jmp_buf1,1);

	//siglongjmp로 호출안하고 그냥 sigsetjmp로 호출한다면
	if(ret==0){
		act_sig1.sa_handler=ssu_signal_handler1;
		sigaction(SIGINT,&act_sig1,NULL);
	}
	//siglongjmp 3으로 호출한다면
	else if(ret==3)
		printf("----------------\n");

	printf("Starting\n");
	//시그널 마스크 env에 저장하는데 1을 썼으 이번엔 2번째
	sigsetjmp(jmp_buf2,2);
	//핸들러 설정
	act_sig2.sa_handler=ssu_signal_handler2;
	//시그널에 관한 액션 설정
	sigaction(SIGUSR1, &act_sig2,NULL);

	//20까지 반복
	for(i=0;i<20;i++){
		printf("i = %d\n",i);
		sleep(1);
	}
	
	exit(0);
}

static void ssu_signal_handler1(int signo){
	//SIGINT 시그널이 오면 sigsetjmp의 리턴 3일 때 실행하는 곳으로 넘어감
	fprintf(stderr,"\nInterrupted\n");
	siglongjmp(jmp_buf1,3);
}

static void ssu_signal_handler2(int signo){
	//SIGUSR1 시그널이 오면 sigsetjmp의 리턴 2일 대 실행하는 곳으로 넘어감
	fprintf(stderr,"\nSIGUSR1\n");
	siglongjmp(jmp_buf2,2);
}

