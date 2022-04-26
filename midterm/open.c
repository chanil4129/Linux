#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void){
	char *fname="ssu_test.txt";
	int fd;

	if((fd=open(fname,O_RDONLY))<0){
		fprintf("open error for %s\n",fname);
		exit(1);
	}

	printf("Success!\n");

	exit(0);
}
