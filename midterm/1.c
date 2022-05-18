#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 128
#define S_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

int main(int argc, char *argv[]){
	char buf[BUFFER_SIZE];
	int fd1, fd2;
	ssize_t size;

	if(argc!=3){
		fprintf(stderr,"usage:%s <file1> <file2>\n",argv[0]);
		exit(1);
	}

	if((fd1=open(argv[1],O_RDONLY))<0){
		fprintf(stderr,"open error for %s\n", argv[1]);
		exit(1);
	}

	if((fd2=open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,S_MODE))<0){
		fprintf(stderr,"open error for %s\n",argv[2]);
		exit(1);
	}

	while((size=read(fd1,buf,BUFFER_SIZE))>0){
		write(fd2,buf,size);
	}

	exit(0);
}
