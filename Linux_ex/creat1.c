#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void){
	char *filename="ssu_test.txt";
	int fd;

	if((fd=creat(filename,0666))<0){
		fprintf(stderr,"error : %s",filename);
		exit(1);
	}
	else{
		printf("Finlename: %s, Descriptor: %d",filename,fd);
		exit(0);
	}

}
