#include <stdio.h>
#include <stdlib.h>

int main(void){
	int sum=0;
	for(int i=0;i<300;i++)
		sum+=i;
	printf("sum:%d\n",sum);
}
