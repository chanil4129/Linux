#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	int i;
	
	if(argc<2){
		fprintf(stderr,"usage:%s <file1> <file2> ... <fileN>\n",argv[0]);
		exit(1);
	}

	for(i=1;i<argv;i++){
		if(access(argv[i],F_OK)<0){
			fprintf(stderr, "%s doesn't exits.\n",argv[i]);
			exit(1);
		}
		if(access(argv[i],R_OK)==0)
			print("User cna read /home/oslab");
	}
	exit(0);
}
