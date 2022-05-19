#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
	int fd;
	char *fname="ssu_test.txt";
	char ch;
	int count=0;
	
	if((fd=open(fname,O_RDONLY))<0){
		fprintf(stderr,"open error for %s\n",fname);
		exit(1);
	}

	while(read(fd,&ch,1)>0){
		if(ch=='m'||ch=='M')
			count++;
	}
	
	printf("m + M = %d\n",count);
	exit(0);
}
