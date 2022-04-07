#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define WORD_MAX 100


int main(void){
	int fd;
	int length=0,offset=0,count=0;
	char buf[WORD_MAX][BUFFER_SIZE];
	char *fname="ssu_test.txt";
	int i,k=0;
	
	if((fd=open(fname,O_RDONLY))<0){
		fprintf(stderr,"open error for %s .\n",fname);
		exit(1);
	}

	/*
	for(length=0;length<WORD_MAX;length++){
		if(read(fd,buf[length],BUFFER_SIZE)<=0){
			break;
		}
		*/
	while((length=read(fd,buf[k],BUFFER_SIZE))>0){
		for(i=0;i<length;i++){
			if(buf[k][i]=='\n'){
				offset=offset+i+1;
				lseek(fd,offset,SEEK_SET);
				count++;
				break;
			}
		}
		k++;
	}
	close(fd);

	for(i=0;i<count;i++)
		printf("%s\n",buf[i]);

	printf("line number : %d \n", count);
	exit(0);
}

