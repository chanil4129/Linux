#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	int fd;

	if(argc!=3){
		fprintf(stderr,"usage: %s <file1> <file2>\n",argv[0]);
		exit(1);
	}

	rename(argv[1],argv[2]);
	
	if((fd=open(argv[1],O_RDONLY))<0){
		printf("second open error for %s\n",argv[1]);
	}
	else{
		fprintf(stderr,"it's very strange");
		exit(1);
	}

	printf("Everything is good!\n");
	close(fd);
	exit(0);
}
