#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>


#define DIRECTORY_SIZE MAXNAMLEN

void print_file_type(struct stat* statbuf){
	char *str;

	if(S_ISREG(statbuf->st_mode))
		str="regular";
	else if(S_ISDIR(statbuf->st_mode))
		str="directory";
	else if(S_ISCHR(statbuf->st_mode))
		str="character special";
	else if(S_ISBLK(statbuf->st_mode))
		str="block special";
	else
		str="unknown mode";

	printf("%s\n",str);
}

int main(int argc,char *argv[]){
	struct dirent *dentry;
	struct stat statbuf;
	DIR *dirp;
	char filename[DIRECTORY_SIZE+1];

	if((dirp=opendir(argv[1]))==NULL||chdir(argv[1])==-1){
		fprintf(stderr,"error\n");
		exit(1);
	}

	while((dentry=readdir(dirp))!=NULL){
		if(dentry->d_ino==0)
			continue;

		memcpy(filename,dentry->d_name,DIRECTORY_SIZE);

		if(lstat(filename,&statbuf)==-1){
			fprintf(stderr,"stat error\n");
			break;
		}

		printf("%s\t: ",filename);
		print_file_type(&statbuf);

	}
	exit(0);
	

}
