#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	if(argc!=3){
		fprintf(stderr,"usage: %s <file1> <file2>\n",argv[0]);
		exit(1);
	}
	if(link(argv[1],argv[2])<0){
		fprintf(stderr,"link error\n");
		exit(1);
	}
	if(unlink(argv[1])<0){
		fprintf(stderr,"unlink error\n");
		exit(1);
	}

	exit(0);
}

