#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define CREAT_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

char buf1[]="123456789";
char buf2[]="abcdefghij";

int main(void){
	char *fame="ssu_test.txt";
	int fd;

	if((fd=creat(fname,CREAT_MODE))<0){
		fprintf(stderr,"creat error for %s\n",fname);
		exit(1);
	}

	if(write(fd,buf1,12)!=12){
		fprintf(stderr,"write error\n");
		exit(1);
	}

	if(lseek(fd,15000,SEEK_CUR)<0){
		fprintf(stderr,"lseek error\n");
		exit(1);
	}

	if(write(fd,buf2,12)!=12){
		fprintf(stderr,"write error\n");
		exit(1);
	}

	exit(0);
}
