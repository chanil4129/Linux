#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

void ssu_setbuf(FILE *fp, char *buf);

int main(void){
	FILE *fp;
	char *fname="/dev/pts/1";
	char buf[BUFFER_SIZE];
	
	if((fp=fopen(fname,"w"))==NULL){
		fprintf(stderr,"fopen error for %s\n",fname);
		exit(1);
	}

	ssu_setbuf(fp,buf);
	fprintf(fp,"hello ");
	sleep(1);
	fprintf(fp,"Linux ");
	sleep(1);
	fprintf(fp,"Program\n");
	sleep(1);
	ssu_setbuf(fp,NULL);
	fprintf(fp,"how ");
	sleep(1);
	fprintf(fp,"are ");
	sleep(1);
	fprintf(fp,"you?\n");
	sleep(1);
	
	exit(0);
}


void ssu_setbuf(FILE *fp, char *buf){
	int fd;
	size_t size;
	int mode;

	fd=fileno(fp);
	if(isatty(fd))
		mode=_IOLBF;
	else
		mode=_IOFBF;
	if(buf==NULL){
		mode=_IONBF;
		size=0;
	}
	else
		size=BUFFER_SIZE;
	setvbuf(fp,buf,mode,size);
}
