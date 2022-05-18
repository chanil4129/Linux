#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

char glob_str[]="write to standard output\n";
int glob_val=10;

int main(void){
	pid_t pid;
	int loc_val;

	loc_val=100;
	if(write(STDOUT_FILENO,glob_str,sizeof(glob_str)-1)!=sizeof(glob_str)-1){
		fprintf(stderr,"write error\n");
		exit(1);
	}
	printf("before fork\n");

