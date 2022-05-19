#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
	struct stat statbuf;
	char *fname="ssu_myfile";
	int fd;

	if((fd=open(fname,O_RDWR|O_CREAT))<0){
		fprintf(stderr,"error\n");
		exit(1);
	}

	close(fd);
	stat(fname,&statbuf);
	printf("# 1st stat call #UID:%d   GID: %d\n",statbuf.st_uid,statbuf.st_gid);

	if(chown(fname,501,300)<0){
		fprintf(stderr,"error\n");
		exit(1);
	}

	stat(fname,&statbuf);
	printf("# 2nd stat call #UID:%d   GID: %d\n",statbuf.st_uid,statbuf.st_gid);

	if(unlink(fname)<0){
		fprintf(stderr,"error");
		exit(1);
	}

	
	stat(fname,&statbuf);
	printf("# 3rd stat call #UID:%d   GID: %d\n",statbuf.st_uid,statbuf.st_gid);

	exit(0);
}

