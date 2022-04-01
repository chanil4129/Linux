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
	char *fname="ssu_test.txt";
	char buf[WORD_MAX][BUFFER_SIZE];
	int i;
	
	if((fd=open(fname,O_RDONLY))<0){
		fprintf(stderr,"open error for %s .\n",fname);
		exit(1);
	}

	for(int k=0;k<WORD_MAX;k++){
		if(read(fd,buf[k],BUFFER_SIZE)<0) break;

		for(i=0;i<BUFFER_SIZE;i++){
			if(buf[k][i]=='\n'){
				offset=offset+i+1;
				lseek(fd,offset,SEEK_SET);
				count++;
				break;
			}
		}
	}
	count--;
	close(fd);

	for(i=0;i<count;i++)
		printf("%s\n",buf[i]);

	printf("line number : %d \n", count);
	exit(0);
}

