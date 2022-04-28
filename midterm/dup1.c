#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int main(void){
	char *fname="ssu_test.txt";
	char buf[BUFFER_SIZE];
	int fd1,fd2;
	int count;

	if((fd1=open(fname,O_RDONLY,0644))<0){
		fprintf(stderr,"open error for %s\n",fname);
		exit(1);
	}

	fd2=dup(fd1);
	count=read(fd1,buf,12);
	buf[count]=0;
	printf("%s",buf);
	lseek(fd1,(long)1,SEEK_CUR);
	count=read(fd2,buf,12);
	buf[count]=0;
	printf("%s",buf);
	exit(0);
}
