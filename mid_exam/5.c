#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>


char* print_time(char ct){
	char *rt="...";
	char buf[20];
	return rt;
}

int main(int argc, char *argv[]){
	struct stat statbuf;
	char mode_bit[11];
	int count=0;

	for(int i=0;i<10;i++)
		mode_bit[i]='-';
	mode_bit[10]='\0';

	if(argc>4){
		fprintf(stderr,"usage: %s <dir1> ... <dirn> \n",argv[0]);
		exit(1);
	}


	for(int i=1;i<argc;i++){
		if(count==0){
			printf("%-12s%-7s%-6s%-7s%-7s%-15s%-15s%-15s%-15s\n","Mode","Blocks","Links","UID","GID","Access","Change","Modify","Path");
			count++;
		}
		if((lstat(argv[i],&statbuf))<0){
			fprintf(stderr, "lstat error\n");
			exit(1);
		}
		if(S_ISDIR(statbuf.st_mode))
			mode_bit[0]='d';

		for(int t=0;t<3;t++){
			if((statbuf.st_mode>>(t*3))&1)
				mode_bit[(2-t)*3+3]='x';
			if((statbuf.st_mode>>(t*3+1))&1)
				mode_bit[(2-t)*3+2]='w';
			if((statbuf.st_mode>>(t*3+2))&1)
				mode_bit[(2-t)*3+1]='r';
		}

		printf("%-12s%-7ld%-6ld%-7ld%-7ld%-15s%-15s%-15s%-15s\n",mode_bit,(long)statbuf.st_blksize,(long)statbuf.st_nlink,(long)statbuf.st_uid,(long)statbuf.st_gid,print_time(statbuf.st_atime),print_time(statbuf.st_ctime),print_time(statbuf.st_mtime),argv[i]);
	//	printf("%-12s%-7ld%-6ld%-7ld%-7ld%-15s%-15s%-15s%-15s\n",mode_bit,(long)statbuf.st_blksize,(long)statbuf.st_nlink,(long)statbuf.st_uid,(long)statbuf.st_gid,print_time(&statbuf.st_atime),print_time(&statbuf.st_ctime),print_time(&statbuf.st_mtime),argv[i]);

			
	}
	exit(0);
}
			

	
