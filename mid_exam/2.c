#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024
#define WORD_MAX 100

int main(void){
	int fd;
	int length=0, offset=0, count=0;
	char *fname="ssu_test.txt";
	//char buf[WORD_MAX][BUFFER_SIZE];
	char buf[BUFFER_SIZE][BUFFER_SIZE];

	if((fd=open(fname,O_RDONLY))<0){
		fprintf(stderr,"open error for %s\n",fname);
		exit(1);
	}

	while((length=read(fd,buf[count],BUFFER_SIZE))>0){
		buf[count][length]='\0';
		for(int i=0;i<length;i++){
			if(buf[count][i]=='\n'){
				offset=offset+1+i;
				lseek(fd,offset,SEEK_SET);
				count++;
				break;
			}
		}
	}

	for(int i=0;i<count;i++){
		printf("%s\n",buf[i]);
	}

	printf("line number : %d\n",count);

	close(fd);
	exit(0);
}
