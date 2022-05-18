#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
	char *fname="ssu_test.txt";
	FILE *fp;
	long fsize;

	if((fp=fopen(fname,"r"))==NULL){
		fprintf(stderr,"fopen error for %s\n",fname);
		exit(1);
	}

	fseek(fp,(long)0,SEEK_END);
	fsize=ftell(fp);
	printf("%ld\n",fsize);
	exit(0);
}
