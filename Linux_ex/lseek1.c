#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(void){
	char *fname="ssu_test.txt";
	int fd;
	off_t fsize;

	if((fd=open(fname,O_RDONLY))<0){
		fprintf(stderr,"open error %s\n",fname);
		exit(1);
	}
	if((fsize=lseek(fd,0,SEEK_END))<0){
		fprintf(stderr,"lseek error %s\n",fname);
		exit(1);
	}
	printf("<%s> is %ld bytes\n",fname,fsize);
	exit(0);
}