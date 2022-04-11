#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>

#define BUFSIZE 1024*16

#if !defined(_OSD_POSIX) && !defined(__DJGPP__)
int read(int, void *, unsigned int);
#endif

//md5 명령어 실행 함수
void command_md5(char *file_path,unsigned char *md){
	FILE *IN;
	MD5_CTX c;
	int fd;
	int i;
	static unsigned char buf[BUFSIZE];

	IN=fopen(file_path,"r");
	if(IN==NULL){
		perror(file_path);
		fprintf(stderr,"md5 error for %s\n",file_path);
		exit(1);
	}

	fd=fileno(IN);
	MD5_Init(&c);
	while(1){
		i=read(fd,buf,BUFSIZE);
		if(i<=0) break;
		MD5_Update(&c,buf,(unsigned long)i);
	}
	MD5_Final(&(md[0]),&c);

	/*
	for(i=0;i<MD5_DIGEST_LENGTH;i++)
		printf("%02x",md[i]);
	printf("\n");
	*/
	fclose(IN);
}
