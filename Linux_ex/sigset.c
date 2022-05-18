#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(void)
{
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);//시그널로 SIGINT를 보냄

	switch (sigismember(&set, SIGINT))//Sigset_t에 시그널 번호로 어떤게 들어 있는지 확인
	{
		case 1 :
			printf("SIGINT is included. \n");
			break;
		case 0 :
			printf("SIGINT is not included. \n");
			break;
		default :
			printf("failed to call sigismember() \n");
	}
	
	switch (sigismember(&set, SIGSYS))//Sigset_t에 시그널 번호로 어떤게 들어 있는지 확인
	{
		case 1 :
			printf("SIGSYS is included. \n");
			break;
		case 0 :
			printf("SIGSYS is not included. \n");
			break;
		default :
			printf("failed to call sigismember() \n");
	}
	exit(0);
}
