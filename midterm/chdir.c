#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
	if(chdir("/etc")<0){
		fprintf(stderr,"chdir errorn\n");
		exit(1);
	}
	printf("chdir \n");
	exit(0);
}

