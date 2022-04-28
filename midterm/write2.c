#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define S_MODE 0644
#define BUFFER_SIZE 1024

int main(int argc,char *argv[]){
	char buf[BUFFER_SIZE];
	int length;
	int fd1,fd2;

	if(argc<3){
		fprintf(stderr,"usage : %s <file>\n",argv[0]);
		exit(1);
	}

	if((fd1=open(argv[1],O_RDONLEY))<0){
		fprintf(stderr,"open error for %d\n",argv[1]);
		exit(1);
	}

	if((fd2=open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,S_MODE))<0){
		fprintf(stderr,"open error for %d\n",argv[2]);
		exit(1);
	}

	if((length=read(fd1,buf,BUFFER_SIZE))>0){
		write(fd2,buf,length);
	}

	exit(0);
}
