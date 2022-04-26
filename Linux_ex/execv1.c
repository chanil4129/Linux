#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/wait.h>

double ssu_maketime(struct timeval *time);
void term_stat(int stat);
void ssu_print_child_info(int stat, struct rusage *rusage);

int main(void){
	//자식 프로세스에서 사용자 모드로 CPU를 사용한 시간과 
	//시스템 모드에서 CPU를 사용한 시간을 출력하는 프로그램
	struct rusage rusage;
	pid_t pid;
	int status;

	//프로세스 생성 및 자식 프로세스 execv으로 실행.
	//usr/bin/find 프로그램을 실행하여 find / -maxdepth 4 -name stdio.h 명령어를 수행한다.
	if ((pid = fork()) == 0) {
		char *args[] = {"find", "/", "-maxdepth", "4", "-name", "stdio.h", NULL};
		if (execv("/usr/bin/find", args) < 0) {
			fprintf(stderr, "execv error\n");
			exit(1);
		}
	}
	//자식 프로세스를 기다리며, 프로세스의 자원 사용에 관한 추가적인 정보를 구조체를 통하여 전달
	//ssu_print_child_info는 그 구조체의 정보를 출력.
	if (wait3(&status, 0, &rusage) == pid)
		ssu_print_child_info(status, &rusage);
	else {
		fprintf(stderr, "wait3 error\n");
		exit(1);
	}
	exit(0);
}

//시간 출력을 위한 함수
double ssu_maketime(struct timeval *time) {
	return ((double)time -> tv_sec + (double)time -> tv_usec/1000000.0);
}

//프로세스가 정상 종료했는지 판단
void term_stat(int stat) {
	//정상종료라면 종료 상태 번호를 출력
	if (WIFEXITED(stat))
		printf("normally terminated. exit status = %d\n", WEXITSTATUS(stat));
	//비정상 종료라면 시그널 번호를 출력
	else if (WIFSIGNALED(stat))
		printf("abnormal termination by signal %d. %s\n", WTERMSIG(stat),
				//코어 파일 생성 여부에 따른 출력
#ifdef WCOREDUMP
				WCOREDUMP(stat)?"core dumped":"no core"
#else
				NULL
#endif
			  );
	//자식 프로세스가 현재 중지 상태이면
	else if (WIFSTOPPED(stat))
		printf("stopped by signal %d\n", WSTOPSIG(stat));
}

//정보 출력
void ssu_print_child_info(int stat, struct rusage *rusage) {
	printf("Termination info follows\n");
	term_stat(stat);
	printf("user CPU time : %.2f(sec)\n", ssu_maketime(&rusage->ru_utime));
	printf("system CPU time : %.2f(sec)\n", ssu_maketime(&rusage->ru_stime));
}
