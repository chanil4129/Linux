#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
	int fd;
	char *fname="ssu_test.txt";

	if((fd=creat(fname,0666))<0){
		fprintf(stderr,"creat error\n");
		exit(1);
	}

	printf("first\n");
	dup2(fd,1);
	printf("second\n");
	exit(0);
}
