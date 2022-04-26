#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
	printf("this is the original program\n");
	//ssu_execl_test_1 프로그램을 실행시킴
    //지금 사용중인 프로세스위에 덮어 실행한 것이므로
    //아래 printf결과는 출력되지 않는다.
	execl("./ssu_execl_test_1", "ssu_execl_test_1","param1", "param2", "param3", (char *)0);
	printf("%s\n", "this line should never get printed\n");

	exit(0);
}
