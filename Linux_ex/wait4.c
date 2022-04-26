#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void){
	//자식 프로세스들의 실행을 종료한 후 부모 프로세스의 출력을 수행하는 프로그램
	pid_t child1, child2;
	int pid, status;

	//새로운 프로세스를 생성하고, 자식프로세스를 새로운 프로그램으로 실행시킴.
	//date와 who 시스템호출 함수를 실행시킨다. 
	if ((child1 = fork()) == 0)
		execlp("date", "date", (char *)0);
	if ((child2 = fork()) == 0)
		execlp("who", "who", (char *)0);
	printf("parent: waiting for children\n");

	//자식 프로세스들이 끝날 때까지 기다리며, 종료된 자식의 pid를 리턴받아 리턴 값에 따라
	//종료된 프로세스들의 종료를 알린다.
	while ((pid = wait(&status)) != -1) {
		if (child1 == pid)
			printf("parent: first child: %d\n", (status >> 8));
		else if (child2 == pid)
			printf("parent: second child: %d\n", (status >> 8));
	}

	printf("parent: all children terminated\n");
	exit(0);
}
