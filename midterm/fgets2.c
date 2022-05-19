#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_MAX 256

int main(void){
	char command[BUFFER_MAX];
	char *prompt="Prompt>>";

	while(1){
		fputs(prompt,stdout);
		if(fgets(command,BUFFER_MAX,stdin)==NULL)
			break;
		system(command);
	}

	fprintf(stdout,"good byt...\n");
	fflush(stdout);
	exit(0);
}
