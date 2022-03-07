#include <stdio.h>
#include <string.h>

int main(void){
	char result[10];
	char *k="fi";
	result[0]='f';
	result[1]='i';
	//result[2]='\0';
	printf("index3 : %d\n",result[2]);
	printf("kindex3: %d\n",k[2]);
	printf("%d\n",strcmp(result,k));
	return 0;
}

