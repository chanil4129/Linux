#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc,char *argv[]){
	if(argc!=3){
		fprintf(stderr,"error\n");
		exit(1);
	}

	if(symlink(argv[1],argv[2])<0){
		fprintf(stderr,"symlink error\n");
		exit(1);
	}
	else
		printf("symlink %s -> %s\n",argv[2],argv[1]);
	exit(0);
}

