#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "employee.h"

#define MAXTRIES 5

int main(int argc,char *argv[]){
	struct flock lock;
	struct employee record;
	int fd, sum=0,try=0;

	if(argc<2){
		fprintf(stderr,"usage : %s file\n",argv[0]);
		exit(1);
	}

	if((fd=open(argv[1],O_RDONLY))<0){
		fprintf(stderr,"open error for %s\n",argv[1]);
		exit(1);
	}

	fcntl(fd,F_GETLK,&lock);
	lock.l_type=F_RDLCK;
	lock.l_whence=0;
	lock.l_start=0l;
	lock.l_len=0l;
	fcntl(fd,F_SETLK,&lock);

	while(1){
		/*
		if(try++<MAXTRIES){
			if(fcntl(fd,F_SETLK,&lock)
			continue;
		}
		*/
		if(try==3)
			break;
		read(fd,(char *)&record,sizeof(record));
		printf("Employee: %s, Salary: %d\n",record.name,record.salary);
		sum+=record.salary;
		try++;
	}
	printf("Total salary: %d\n",sum);

	sleep(10);
}

