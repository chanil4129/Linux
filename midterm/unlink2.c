#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
	char buf[64];
	char *fname="ssu_tempfile";
	int fd;
	int length;

	if((fd=open(fname,O_RDWR|O_CREAT|O_TRUNC, 0600))<0){
		fprintf(stderr,"error\n");
		exit(1);
	}

	if(unlink(fname)<0){
		fprintf(stderr,"error\n");
		exit(1);
	}

	if(write(fd,"how are you?",12)!=12){
		fprintf(stderr,"error\n");
		exit(1);
	}

	lseek(fd,0,0);
	if((length=read(fd,buf,sizeof(buf)))<0){
		fprintf(stderr,"buf error\n");
		exit(1);
	}
	else
		buf[length]=0;
	printf("%s\n",buf);

	if((fd=open(fname,O_RDWR))<0){
		fprintf(stderr,"second open error\n");
		exit(1);
	}
	close(fd);
	exit(0);
}

