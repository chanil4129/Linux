#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main(void){
	char *filename="ssu_test.txt";
	int fd;

	if((fd=open(filename,O_RDONLY))<0){
	   	fprintf(stderr,"error : %s",filename);
		exit(1);
	}
	else{
		printf("%s  %d",filename,fd);
		exit(0);
	}
}
