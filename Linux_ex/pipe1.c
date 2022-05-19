#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 1024

int main(void){
	char buf[BUFFER_SIZE];
	int pipe_fd[2];

	//파이프 파일 생성(단방향 IPC 사용), 리턴값으로는 0과 1이 나옴
	if (pipe(pipe_fd) == -1) {
		fprintf(stderr, "pipe error\n");
		exit(1);
	}

	//pipe_fd[1]은 4번 파일디스크립터로 write만 함
	printf("writing to file descriptor #%d \n", pipe_fd[1]);
	write(pipe_fd[1], "OSLAB", 6);
	//pipe_fd[0]은 3번 파일디스크립터로 read만 함
	printf("reading from file descriptor #%d \n", pipe_fd[0]);
	read(pipe_fd[0], buf, 6);
	//통신하므로 OSLAB 출력
	printf("read \"%s \" \n", buf);
	exit(0);
}
