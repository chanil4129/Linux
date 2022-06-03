#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

void set_flags(int fd,int flags);
void clr_flags(int fd,int flags);

char buf[50000];

int main(void){
	int nwrite,ntowrite;
	char *ptr;

	ntowrite=read(STDIN_FILENO,buf,sizeof(buf));
	fprintf(stderr,"reading %d bytes\n",ntowrite);

	set_flags(STDOUT_FILENO,O_NONBLOCK);

	ptr=buf;
	while(ntowrite>0){
		errno=0;
		nwrite=write(STDOUT_FILENO,buf,ntowrite);
		fprintf(stderr,"nwrite=%d, errno=%d\n",nwrite,errno);

		if(nwrite>0){
			ptr+=nwrite;
			ntowrite-=nwrite;
		}
	}
	clr_flags(STDOUT_FILENO,O_NONBLOCK);
}

void set_flags(int fd,int flags){
	int val;

	if((val=fcntl(fd,F_GETFL,0))<0){
		fprintf(stderr,"fcntl error\n");
		exit(1);
	}
	val|=flags;
	if(fcntl(fd,F_SETFL,val)<0){
		fprintf(stderr,"fcntl error\n");
		exit(1);
	}
}

void clr_flags(int fd, int flags){
	int val;

	if((val=fcntl(fd,F_GETFL,0))<0){
		fprintf(stderr,"fcntl error\n");
		exit(1);
	}
	val&=~flags;
	if(fcntl(fd,F_SETFL,val)<0){
		fprintf(stderr,"fcntl error\n");
		exit(1);
	}
}


