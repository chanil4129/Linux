#include <stdio.h>
#include <stdlib.h>

int main(void) {
	//이것만 출력하고 abort()로 비정상 종료
	printf("abort terminate this program\n");
	abort();
	//이것은 출력 안됨
	printf("this line is never reached\n");
	exit(0);
}
