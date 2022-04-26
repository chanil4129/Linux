#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
	char *fname="ssu_text.txt";
	int fd;
	int fsize;

	if((fd=open(fname,O_WRONLY))<0){
		fprintf(stderr,"open error for %s\n",fname);
		exit(1);
	}
	
	if((fsize=lseek(fd,(off_t)0,SEEK_END))<0){
		fprintf(stderr,"lseek error\n");
		exit(1);
	}

	print("%d",fsize);
	exit(0);
}
