#include <dirent.h> 
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 

#define TABLE_SIZE 1024
#define BUFFER_SIZE 1024
#define add(a,b) O1


int main(void){
	int fd;
	int length;
	char buf[100];

	if(fd=open("b",O_RDONLY));
	if((length=read(fd,buf,10)>0));
	printf("length: %d\n",length);
	printf("%c\n",buf[0]);
}
	
