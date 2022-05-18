#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

void ssu_timestamp(char *str);

int main(void){
	unsigned int ret;

	ssu_timestamp("before sleep()");
	//ret에는 10초에서 타이머를 재며 남은 시간이 저장됨
	ret = sleep(10);
	ssu_timestamp("after sleep()");
	printf("sleep() returned %d\n", ret);
	exit(0);
}

void ssu_timestamp(char *str) {
	time_t time_val;

	time(&time_val);
	printf("%s the time is %s\n", str, ctime(&time_val));
}
