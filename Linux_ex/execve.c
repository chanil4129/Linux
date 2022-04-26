#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){
	char *argv[] = {"ssu_execl_test_1", "param1", "param2", (char *)0};
	char *env[] = {"NAME = value","nextname=nextvalue","HOME=/home/oslab",(char *)0};

	printf("this is the original program\n");
	//execve()를 이용하여 수행할 프로그램의 환경 변수도 저장한다.
	//execve()의 인자인 argv는 execve()로 수행한 프로세스의 main()의
	//두 번째 인자인 argv가 되고, envp는 수행한 프로세스의 환경 변수가 된다.
    //지금 사용중인 프로세스위에 덮어 실행한 것이므로
    //아래 printf결과는 출력되지 않는다.
	execve("./ssu_execl_test_1", argv, env);
	printf("%s\n", "This line should never get printed\n");
	exit(0);
}
