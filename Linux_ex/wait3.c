#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void){
	//fork로 프로세스를 생성하고, 자식프로세스를 execl로 새프로그램으로 대체
	//새로운 프로그램은 다음 echo 시스템 호출 함수를 사용하여 메세지를 출력한다.
	//부모 프로세스가 wait 하는 동안 자식 프로세스는 경로 또는 파일에 의해 지정된
	//한 이미지 파일로부터 생성되어진다. 현재 프로세스의 이미지는 디스크에서 가져온
	//새 프로그램의 이미지로 대체된다.
	if(fork()==0)
		execl("/bin/echo","echo","this is","message one",(char *)0);

	if(fork()==0)
		execl("/bin/echo","echo","this is","message Two",(char *)0);

	printf("parent: waiting for children\n");

	//자식 프로세스가 있다면 wait()를 정상적으로 호출하며 무한루프에 빠지지 않는다.
	while(wait((int *)0)!=-1);

	//자식 프로세스의 출력을 먼저 수행한 후 부모 프로세스의 출력을 수행
	printf("parent: all children terminated\n");
	exit(0);
}
