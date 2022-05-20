#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main(void) {
	char buf[BUFFER_SIZE];
	int pid;
	int pipe_fd[2];

	//파이프 파일 생성
	pipe(pipe_fd);
	//프로세스 생성
	if ((pid = fork()) < 0) {
		fprintf
			(stderr, "fork error\n");
		exit(1);
	}
	//부모 프로세스는 write를 담당
	else if (pid > 0) {
		printf(" PARENT: writing to the pipe\n");
		write(pipe_fd[1], "OSLAB", 6);
		printf(" PARENT: waiting\n");
		//쓰기 작업 후 자식 프로세스를 기다리고 자식프로세스가 작업을 마치면 같이 종료
		wait(NULL);
	} 
	//자식 프로세스는 read를 담당
	else {
		printf(" CHILD: reading from pipe\n");
		read(pipe_fd[0], buf, 6);
		printf(" CHILD: read\"%s\"\n", buf);
		exit(0); 
	}
	//pipe 파일을 통해 프로세스가 단방향 통신을 하므로 자식 프로세스에서 출력이 가능

	exit(0); 
}
