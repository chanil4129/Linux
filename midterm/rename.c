#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	int fd;
	if(argc!=3){
		fprintf(stderr,"usage\n");
		exit(1);
	}

	if(rename(argv[1],argv[2])<0){
		fprintf(stderr,"rename error\n");
		exit(1);
	}

	if((fd=open(argv[1],O_RDONLY))<0)
		printf("second..\n");
	else{
		fprintf(stderr,"what?\n");
		exit(1);
	}

	if((fd=open(argv[2],O_RDONLY))<0){
		fprintf(stderr,"error\n");
		exit(1);
	}

	printf("good\n");
	exit(0);
}


