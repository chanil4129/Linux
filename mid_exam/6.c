#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
	char *fname="ssu_line.txt";
	char *frname="ssu_blank.txt";
	int fd_write, fd_read;
	int linecnt=0;
	int wordcnt=0;
	int charcnt=0;
	char buf[200];
	int i=0;

	if((fd_read=open(frname,O_RDONLY))<0){
		fprintf(stderr,"open error for %s\n",frname);
		exit(1);
	}
	if((fd_write=open(fname,O_WRONLY|O_CREAT|O_TRUNC,0644))<0){
		fprintf(stderr,"open error for %s\n",fname);
		exit(1);
	}

	dup2(fd_read,0);
	if(read(fd_read,buf,200)<0){
		fprintf(stderr,"read error\n");
		exit(1);
	}
	dup2(fd_write,1);
	dup2(1,fd_write);
	while(scanf("%s",buf)!=EOF){
		printf("%s",buf);
	}

	
		

	


	close(fd_write);
	close(fd_read);
	exit(0);
}
