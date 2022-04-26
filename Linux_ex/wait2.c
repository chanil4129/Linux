#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define EXIT_CODE 1

int main(void){
	//프로세스들의 상태를 출력하는 프로그램
	pid_t pid;
	int ret_val, status;

	//프로세스 생성 및 자식 프로세스 실행
	if ((pid = fork()) == 0) {
		printf("child: pid = %d ppid = %d exit_code = %d\n",
				getpid(), getppid(), EXIT_CODE);//getpid()는 자식 프로세스의 pid, getppid()는 부모 프로세스의 pid
		exit(EXIT_CODE);
	}

	//exit() 인자의 하위 8비트는 wait()의 status 인자로 전달.
	//실행 결과와 같이 자식 프로세스가 종료된 후 부모 프로세스가 출력.
	//status>>8을 통해 상위 비트가 1이된다. 즉 출력하면 1이 나온다.
	printf("parent: waiting for child = %d\n", pid);
	ret_val = wait(&status);
	printf("parent: return value = %d, ", ret_val);
	printf(" child's status = %x", status);
	printf(" and shifted = %x\n", (status >> 8));
	exit(0);
}
