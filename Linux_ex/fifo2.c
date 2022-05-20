#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024
#define FIFO_NAME "ssu_fifofile"

int main(void){
	char buf[BUFFER_SIZE];
	int fd;
	int length;

	mknod(FIFO_NAME, S_IFIFO | 0666, 0);
	printf("waiting for writers... \n");

	if((fd=open(FIFO_NAME,O_RDONLY))<0){
		fprintf(stderr,"open error for %s\n",FIFO_NAME);
		exit(1);
	}

	printf("got a writer \n");

	do {
		if((length=read(fd,buf,BUFFER_SIZE))==-1){
			fprintf(stderr,"read error\n");
			exit(1);
		}
		else {
			buf[length]='\0';
			printf("tick: read %d bytes: \"%s\"\n",length,buf);
		}
	} while (length > 0);

	exit(0);
}
