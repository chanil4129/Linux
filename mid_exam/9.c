#include <stdio.h>
#include <stdlib.h>

typedef struct _person{
	char name[10];
	int age;
	double height;
}Person;

int main(void){
	FILE *fp;
	int i, res;
	Person ary[3]={{"Hong GD",500,175.4},
		{"Lee SS",350,180.0},
		{"King SJ",500,178.6}};
	Person tmp;

	if((fp=fopen("ftest.txt","w"))==NULL){
		fprintf(stderr,"open error for %s\n","ftest.txt");
		exit(1);
	}

	fwrite(&ary,sizeof(ary),1,fp);
	fclose(fp);

	if((fp=fopen("ftest.txt","r"))==NULL){
		fprintf(stderr,"open error for %s\n","ftest.txt");
		exit(1);
	}

	printf("[ First print ]\n");
	while(fread(&tmp,sizeof(tmp),1,fp)!=0)
		printf("%s %d %.2lf\n",tmp.name,tmp.age,tmp.height);
	fseek(fp,0,SEEK_SET);
	printf("[ Second print ]\n");
	while(fread(&tmp,sizeof(tmp),1,fp)!=0)
		printf("%s %d %.2lf\n",tmp.name,tmp.age,tmp.height);


	fclose(fp);
	exit(0);
}
