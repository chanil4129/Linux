#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

int main(void){
	char operator;
	FILE *fp;
	int character;
	int number=0;
	if((fp=fopen("ssu_expr.txt","r"))==NULL){
		fprintf(stderr,"error\n");
		exit(1);
	}
	while(!feof(fp)){
		while((character=fgetc(fp))!=EOF&&isdigit(character))
			number=10*number+character-48;
		fprintf(stdout,"%d\n",number);
		number=0;
		
		if(character!=EOF){
			/*
			ungetc(character,fp);
			operator=fgetc(fp);
			*/
			operator=character;
			printf("Operator=>%c\n",operator);
		}
	}
	fclose(fp);
	exit(0);
}
