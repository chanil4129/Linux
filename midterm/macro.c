#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	struct stat file_info;
	char *str;
	int i;

	for(i=1;i<argc;i++){
		printf("name = %s, type = ",argv[i]);
		if(lstat(argv[i],&file_info)<0){
			fprintf(stderr, "lstat error\n");
			continue;
		}

		if(S_ISREG(file_info.st_mode))
			str="regular";
		else if(S_ISDIR(file_info.st_mode))
			str="direcory";
		else
			str="unknown mode";
		print("%s\n",str);

	}
	exit(0);
}
