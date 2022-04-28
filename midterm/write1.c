#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int main(void){
	char buf[BUFFER_SIZE];
	int length;

	length=read(0,buf,BUFFER_SIZE);
	write(1,buf,length);
	exit(0);
}
