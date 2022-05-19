#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//사용할 파프의 이름 정하기
#define FIFO_NAME "american_maid"

int main(void){
	char s[300];
	int num, fd;

	//FIFO 파일 성할 때 mknod나 mkfifo 둘 중 하나 쓸 수 있음. mkfifo를 권장
	//송/수신 프로세스를 명확하게 지정할 때 사용(named pipe)
	//mknod(FIFO_NAME, S_IFIFO | 0666, 0);
	mkfifo(FIFO_NAME, S_IWUSR|S_IRUSR|S_IWGRP|S_IRGRP|S_IWOTH|S_IROTH);
	printf("waiting for readers... \n");
	fd = open(FIFO_NAME, O_WRONLY);
	printf("got a reader--type some stuff \n");
	while (gets(s), !feof(stdin)) {
		if ((num = write(fd, s, strlen(s))) == -1)
			perror("write");
		else
			printf("speak: wrote %d bytes \n", num);
	}

	exit(0);
}
