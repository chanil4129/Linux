#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int main(void){
	char *fname="ssu_test.txt";
	int fd;

	if((fd=creat(fname,0666))<0){
		fprintf(stderr,"creat error for %s\n",fname);
		exit(1);
	}
	else{
		printf("success!\n");
		fclose(fd);
	}

	exit(0);
}
