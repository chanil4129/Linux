#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int main(){
	char buf[BUFFER_SIZE];
	char *fname="ssu_test.txt";
	int fd;

	if((fd=open(fname,O_RDONLY))<0){
		fprintf(stderr,"open error for %s\n",fname);
		exit(1);
	}

	dup2(fd,0);

	while(scanf("%s",buf)!=EOF)
		printf("fd scanf : %s\n",buf);
		

	exit(0);
}

