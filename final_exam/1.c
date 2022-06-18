#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int main(void){
	int val;
	int fd;

	if((fd=open("ssu_test.txt",O_RDONLY))<0){
		fprintf(stderr,"open error %s\n","ssu_test.txt");
		exit(1);
	}
	val=fcntl(fd,F_GETFL,0);
	if(val&FD_CLOEXEC)
		printf("close-on-exec bit on\n");
	else{
		printf("close-on-exec bit off\n");
	}
	val|=FD_CLOEXEC;
	fcntl(fd,F_SETFL,val);
	if(val&FD_CLOEXEC)
		printf("close-on-exec bit on\n");
	
	execl("/home/lsp/loop","./loop",NULL);
	
	sleep(5);
	exit(0);
}
