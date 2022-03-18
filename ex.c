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

struct test{
	int num;
	char *buf1;
	char buf2[1024];
};
	

int main(void){
	char buf[1023];
	strcpy(buf,"12345");
	printf("%d\n",strcmp(buf,"12345"));
	buf[5]='\n';
	printf("%d",strcmp(buf,"12345"));
}
	
