#include <stdio.h>
#include <stdlib.h>

void print_size(long long size,char *p_size){
    char temp[26];
    char *p;
    int i;

    p=temp;
    for(i=0;i<20&&size>0;i++){
        if(i&&(i%3)==0)
            *p++=',';
        *p++=(size%10)+'0';
        size/=10;
    }
    p--;
	printf("psize:%ld\n",p-temp);
    while(p>=temp)
        *p_size++=*p--;
    *p_size=0;
}

int main(void){
	char str[26];
	long long a=12345634567891;
	print_size(a,str);
	printf("%lld\n",a);
	printf("%s",str);
	exit(0);
}
