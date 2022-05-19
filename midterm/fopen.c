#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
	char *fname="ssu_test.txt";

	if(fopen(fname,"r")==NULL){
		fprintf(stderr,"error");
		exit(1);
	}

	if(freopen(fname,"w",stdout)!=NULL){
		printf("Linux Programming\n");
	}

	exit(0);
}
