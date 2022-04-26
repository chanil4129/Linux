#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){
	//자식프로세스 생성하여 echo 시스템호출 명령어 실행
	if (fork() == 0) {
		execl("/bin/echo", "echo", "this is", "message one", (char *)0);
		fprintf(stderr, "exec error\n");
		exit(1);
	}
	if (fork() == 0) {
		execl("/bin/echo", "echo", "this is", "message two", (char *)0);
		fprintf(stderr, "exec error\n");
		exit(1);
	}
	if (fork() == 0) {
		execl("/bin/echo", "echo", "this is", "message three", (char *)0);
		fprintf(stderr, "exec error\n");
		exit(1);
	}
	//execl이 수행되면서 원래 프로세스를 덮어 써버리기 때문에 아래 printf는 출력안된다.
	printf("Parent program ending\n");

	exit(0);
}
