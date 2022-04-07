#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024

int main(void){
	char *fname="ssu_test.txt";
	int fd1,fd2;
	int count;
	char buf[BUFFER_SIZE];
	char c;

	if((fd1=open(fname,O_RDONLY,0644))<0){
		fprintf(stderr,"open error for %s\n",fname);
		exit(1);
	}

	if((count=lseek(fd1,0,SEEK_END))<0){
		fprintf(stderr,"lseek error\n");
		exit(1);
	}
	printf("%d\n",count);
	lseek(fd1,0,SEEK_SET);
	count=read(fd1,buf,BUFFER_SIZE);
	fd2=dup(fd1);
	close(fd1);
//	write(1,buf,count); //fd2를 이용해서 출력하는 의도인 것 같아서 지금 줄만 써도 되는데 따로 추가했습니다.
	if((fd2=open(fname,O_RDWR,0644))<0){
		fprintf(stderr,"open error for %s\n",fname);
		exit(1);
	}
	write(fd2,buf,count);
	if(lseek(fd2,0,SEEK_SET)<0){
		fprintf(stderr,"lseek error\n");
		exit(1);
	}
	while(read(fd2,&c,1)>0){
		putchar(c);
	}
	close(fd2);
	exit(0);
}

