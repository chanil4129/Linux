#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	if(argc!=3){
		fprintf(stderr,"usage\n");
		exit(1);
	}
	
	if(link(argv[1],argv[2])<0){
		fprintf(stderr,"linkerror\n");
		exit(1);
	}
	else
		printf("step1 passed.\n");

	if(remove(argv[1])<0){
		fprintf(stderr, "remove error\n");
		remove(argv[2]);
		exit(1);
	}
	else
		printf("step2 passed.\n");

	printf("end\n");
	exit(0);
}
