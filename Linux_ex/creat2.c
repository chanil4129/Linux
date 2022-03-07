#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(void){
	char *fname="ssu_test.txt";
	int fd;

	if((fd=creat(fname,0066))<0){
		fprintf(stderr,"error %s",fname);
		exit(1);
	}
	else{
		close(fd);
		fd=open(fname,O_RDWR);
		printf("<%s>",fname);
	}
	exit(0);
}
