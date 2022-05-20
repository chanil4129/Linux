#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main(void){
	char buf[BUFFER_SIZE];
	int pid;
	int pipe_fd[2];

	//pipe 파일 생성
	if (pipe(pipe_fd) < 0) {
		fprintf(stderr, "pipe error\n");
		exit(1);
	}
	//프로세스 생성
	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	//자식 프로세스가 할 일
	else if (pid == 0) {
		close(pipe_fd[0]);
		while (1) {
			memset(buf, 0x00, BUFFER_SIZE);
			sprintf(buf, "Hello Mother Process. My name is %d\n", getpid());
			write(pipe_fd[1], buf, strlen(buf));
			sleep(1);
		}
	}
	//부모 프로세스가 할 일
	else {
		close(pipe_fd[1]);
		while (1) {
			memset(buf, 0x00, BUFFER_SIZE);
			read(pipe_fd[0], buf, BUFFER_SIZE);
			fprintf(stderr, "%s", buf);
		}
	}
	//자식 프로세스는 쓰기만하고 부모프로세스는 읽기만 한다.

	exit(0);
}
