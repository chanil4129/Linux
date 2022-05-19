#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	FILE *fp;
	int character;

	if((fp=fopen(argv[1],"r"))==NULL){
		fprintf(stderr,"error\n");
		exit(1);
	}
	
	character=fgetc(fp);
	while(!feof(fp)){
		fputc(character,stdout);
		if(ferror(fp)){
			fprintf(stderr,"error\n");
			clearerr(fp);
		}
		character=fgetc(fp);
	}
	fclose(fp);
	exit(0);
}
		
