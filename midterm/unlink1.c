#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
	char *fname="ssu_dump.txt";
	if(open(fname,O_RDWR)<0){
		fprintf(stderr,"open error\n");
		exit(1);
	}
	if(unlink(fname)<0){
		fprintf(stderr,"errorn\n");
		exit(1);
	}

	printf("File unlinked\n");
	sleep(20);
	printf("Done\n");
	exit(0);
}
