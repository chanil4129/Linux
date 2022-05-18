#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define MODE_EXEC (S_IXUSR|S_IXGRP|S_IXOTH)

int main(int argc,char *argv[]){
	struct stat statbuf;
	int i;
	
	if(argc<2){
		fprintf(stderr,"error");
		exit(1);
	}

	for(i=1;i<argc;i++){
		if(stat(argv[i],&statbuf)<0){
			fprintf(stderr,"error");
			continue;
		}
		
		statbuf.st_mode|=MODE_EXEC;
		statbuf.st_mode^=(S_IXGRP|S_IXOTH);
		if(chmod(argv[i],statbuf.st_mode)<0)
			fprintf(stderr,"error\n");
		else
			printf("%s : file permission was chanaged.\n",argv[i]);
	}
	exit(0);
}

