#include <dirent.h> 
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 



int main(void){
	unsigned char flag=0;
	printf("%d\n",flag|=1);
	printf("%d\n",flag|=2);
	printf("%d\n",flag|=3);
	printf("%d\n",flag&1);
	printf("%d\n",flag&~1);
	flag=7;
	printf("%d\n",flag^=2);
}
	
