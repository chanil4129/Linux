#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFLEN 512
#define QNUM 5

void print_wrong_answer(char *filename);

int main(int argc, char *argv[]){
	if(argc<2){
		fprintf(stderr,"usage: %s <file1> \n",argv[0]);
		exit(1);
	}
	print_wrong_answer(argv[1]);
}

void print_wrong_answer(char *filename){
	FILE *fp;
	char question_name[QNUM][15];
	char line[BUFLEN];
	char *ptr;
	int question_num=0;
	int error_cnt;

	if((fp=fopen(filename,"r"))==NULL){
		fprintf(stderr,"fopen error for %s\n",filename);
		exit(1);
	}

	

	fscanf(fp,"%s\n",line);
	strcpy(question_name[question_num++],strtok(line,","));

	while((ptr=strtok(NULL,","))!=NULL)
		strcpy(question_name[question_num++],ptr);
//	printf("%s\n",question_name[0]);
	while(fscanf(fp,"%s\n",line)!=EOF){
		question_num=0;
		error_cnt=0;

		ptr=strtok(line,",");
		printf("%s's wrong answer: ",ptr);

		while((ptr=strtok(NULL,","))!=NULL){
			if(atof(ptr)==0){
				printf("%s     ",question_name[question_num]);
				error_cnt++;
			}
			question_num++;
		}

		if(error_cnt>0)
			printf("\nThe number of wrong answer: %d\n",error_cnt);
		else
			printf("perfect!!\n");

	}

	if(fclose(fp)){
		fprintf(stderr,"fclose error\n");
		exit(1);
	}
}
