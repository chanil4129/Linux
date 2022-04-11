#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>

#define BUFSIZE	1024*16

#ifndef _OSD_POSIX
int read(int, void *, unsigned int);
#endif

//sha1 명령어 실행 함수
void command_sha1(char *file_path,unsigned char *md){
	FILE *IN;
    SHA_CTX c;
    int fd;
    int i;
    static unsigned char buf[BUFSIZE];

    IN=fopen(file_path,"r");
    if(IN==NULL){
        perror(file_path);
        fprintf(stderr,"sha1 error for %s\n",file_path);
        exit(1);
    }

    fd=fileno(IN);
    SHA1_Init(&c);
    while(1){
        i=read(fd,buf,BUFSIZE);
        if(i<=0) break;
        SHA1_Update(&c,buf,(unsigned long)i);
    }
    SHA1_Final(&(md[0]),&c);

	/*
    for(i=0;i<SHA_DIGEST_LENGTH;i++)
        printf("%02x",md[i]);
    printf("\n");
	*/
    fclose(IN);

}

