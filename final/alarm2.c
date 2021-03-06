close_on_exec.c                                                                                     0000664 0001750 0001750 00000001156 14244666014 012447  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
int main(void)
{
	int fd;
	int val;
	if ((fd = open("exec_copy.txt", O_CREAT)) < 0) {
		fprintf(stderr, "open error for %s\n", "exec_copy.txt");
		exit(1);
	}
	val = fcntl(fd, F_GETFD, 0);
	if (val & FD_CLOEXEC)
		printf("close-on-exec bit on\n");
	else
		printf("close-on-exec bit off\n");
	val |= FD_CLOEXEC;
	if (val & FD_CLOEXEC)
		printf("close-on-exec bit on\n");
	else
		printf("close-on-exec bit off\n");
	fcntl(fd, F_SETFD, val);
	execl("/home/chan/Linux/Linux_ex/loop", "./loop", NULL);
}
                                                                                                                                                                                                                                                                                                                                                                                                                  closeonexec3.c                                                                                      0000664 0001750 0001750 00000000462 14244605713 012232  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

int main(void){
	int flag;
	if((flag=fcntl(STDOUT_FILENO, F_DUPFD))== -1){
		fprintf(stderr, "Error : Checking CLOSE_ON_EXEC\n");
		exit(1);
	}

	printf("CLOSE ON EXEC flag is = %d\n", flag);
	exit(0);
}
                                                                                                                                                                                                              daemon.c                                                                                            0000664 0001750 0001750 00000001453 14246067572 011113  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int ssu_daemon_init(void);

int main(void){
	pid_t pid;

	pid=getpid();
	printf("pid : %d\n",pid);
	printf("daemon init\n");
	if(ssu_daemon_init()<0){
		fprintf(stderr,"daemon error\n");
		exit(1);
	}
	exit(0);
}

int ssu_daemon_init(void){
	pid_t pid;
	int fd,maxfd;

	if((pid=fork())<0){
		fprintf(stderr,"fork error\n");
		exit(1);
	}
	else if(pid!=0)
		exit(0);

	pid=getpid();
	printf("pid : %d\n",pid);
	setsid();
	signal(SIGTSTP,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGTTOU,SIG_IGN);
	maxfd=getdtablesize();

	for(fd=0;fd<maxfd;fd++)
		close(fd);

	umask(0);
	chdir("/");
	fd=open("/dev/null",O_RDWR);
	dup(0);
	dup(0);
	return 0;
}
                                                                                                                                                                                                                     ex7-2.c                                                                                             0000664 0001750 0001750 00000002204 14245716130 010473  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1=PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2=PTHREAD_COND_INITIALIZER;
int length;
int buf[100];

void *ssu_thread_producer(void *arg){
	int i;

	for(i=1;i<=300;i++){
		pthread_mutex_lock(&mutex);
		buf[length++]=i;

		if(length==100)
			pthread_cond_wait(&cond1,&mutex);

		pthread_cond_signal(&cond2);
		pthread_mutex_unlock(&mutex);
	}
}

void *ssu_thread_consumer(void *arg){
	int i;
	int sum=0;

	for(i=1;i<=300;i++){
		pthread_mutex_lock(&mutex);

		if(length==0)
			pthread_cond_wait(&cond2,&mutex);

		sum+=buf[--length];
		pthread_cond_signal(&cond1);
		pthread_mutex_unlock(&mutex);

	}

	printf("%d\n",sum);
}

int main(void){
	pthread_t producer_tid, consumer_tid;

	pthread_create(&producer_tid,NULL,ssu_thread_producer,NULL);
	pthread_create(&consumer_tid,NULL,ssu_thread_consumer,NULL);
	pthread_join(producer_tid,NULL);
	pthread_join(consumer_tid,NULL);

	//mutex,cond ?????????
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond1);
	pthread_cond_destroy(&cond2);

	exit(0);
}

                                                                                                                                                                                                                                                                                                                                                                                            exam.c                                                                                              0000664 0001750 0001750 00000000264 14246135345 010573  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static void ssu_handler(int signal);

int main(void){
	
                                                                                                                                                                                                                                                                                                                                            fcntl8.c                                                                                            0000664 0001750 0001750 00000001735 14246141667 011047  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "ssu_employee.h"

#define DUMMY 0

int main(int argc,char *argv[]){
	struct ssu_employee record;
	int fd;
	int flags;
	int length;
	int pid;

	if(argc<2){
		fprintf(stderr,"Usage : %s file\n",argv[0]);
		exit(1);
	}
	
	if((fd=open(argv[1],O_RDWR))<0){
		fprintf(stderr,"open error for %s\n",argv[1]);
		exit(1);
	}

	if((flags=fcntl(fd,F_GETFL,DUMMY))==-1){
		fprintf(stderr,"fcntl F_GETFL error\n");
		exit(1);
	}

	flags|=O_APPEND;

	if(fcntl(fd,F_SETFL,flags)==-1){
		fprintf(stderr,"fcntl F_SETFL error\n");
		exit(1);
	}

	pid=getpid();

	while(1){
		printf("Enter employee name : ");
		scanf("%s", record.name);

		if(record.name[0]=='.')
			break;

		printf("Enter employee salary : ");
		scanf("%d",&record.salary);
		record.pid=pid;
		length=sizeof(record);

		if(write(fd,(char *)&record,length)!=length){
			fprintf(stderr,"record write error\n");
			exit(1);
		}
	}
	close(fd);
	exit(0);
}
                                   fcntl9.c                                                                                            0000664 0001750 0001750 00000001633 14245736555 011052  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define NAMESIZE 50
#define MAXTRIES 5

struct employee{
	char name[NAMESIZE];
	int salary;
	int pid;
};

int main(int argc,char *argv[]){
	struct flock lock;
	struct employee record;
	int fd,sum=0,try=0;

	sleep(10);

	if((fd=open(argv[1],O_RDONLY))==-1){
		perror(argv[1]);
		exit(1);
	}

	lock.l_type=F_RDLCK;
	lock.l_whence=0;
	lock.l_start=0L;
	lock.l_len=0L;

	while(fcntl(fd,F_SETLK,&lock)==-1){
		if(errno==EACCES){
			if(try++<MAXTRIES){
				sleep(1);
				continue;
			}
			printf("%s busy -- try later\n",argv[1]);
			exit(2);
		}
		perror(argv[1]);
		exit(3);
	}
	sum=0;
	while(read(fd,(char *)&record,sizeof(record))>0){
		printf("Employee: %s, Salary: %d\n",record.name,record.salary);
		sum+=record.salary;
	}
	printf("\nTotal salary: %d\n",sum);

	lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&lock);
	close(fd);
}

                                                                                                     fcntl_lock1.c                                                                                       0000664 0001750 0001750 00000001631 14246152162 012033  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define NAMESIZE 50
#define MAXTRIES 5

struct employee{
	char name[NAMESIZE];
	int salary;
	int pid;
};

int main(int argc,char *argv[]){
	struct flock lock;
	struct employee record;
	int fd,sum=0,try=0;

	sleep(5);

	if((fd=open(argv[1],O_RDONLY))==-1){
		perror(argv[1]);
		exit(1);
	}

	lock.l_type=F_RDLCK;
	lock.l_whence=0;
	lock.l_start=0L;
	lock.l_len=0L;

	while(fcntl(fd,F_SETLK,&lock)==-1){
		if(errno==EACCES){
			if(try++<MAXTRIES){
				sleep(1);
				continue;
			}
			printf("%s busy -- try later\n",argv[1]);
			exit(2);
		}
		perror(argv[1]);
		exit(3);
	}
	sum=0;
	while(read(fd,(char *)&record,sizeof(record))>0){
		printf("Employee: %s, Salary: %d\n",record.name,record.salary);
		sum+=record.salary;
	}
	printf("\nTotal salary: %d\n",sum);

	lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&lock);
	close(fd);
}
                                                                                                       fcntl_lock2.c                                                                                       0000664 0001750 0001750 00000001170 14246031411 012023  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	struct flock lock;
	int fd;
	char command[100];
	if((fd = open(argv[1], O_RDWR)) == -1) {
		perror(argv[1]);
		exit(1);
	}
	lock.l_type = F_WRLCK;
	lock.l_whence = 0;
	lock.l_start = 0l;
	lock.l_len = 0l;
	if(fcntl(fd, F_SETLK, &lock) == -1) {
		if (errno == EACCES) {
			printf("%s busy -- try later\n", argv[1]);
			exit(2);
		}
		perror(argv[1]);
		exit(3);
	}
	sprintf(command, "vim %s\n", argv[1]);
	system(command);
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);
	close(fd);
	return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                                        fcntl_lock3.c                                                                                       0000664 0001750 0001750 00000002155 14246032266 012041  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define NAMESIZE 50

struct employee{
	char name[NAMESIZE];
	int salary;
	int pid;
};

int main(int argc,char *argv[]){
	struct flock lock;
	struct employee record;
	int fd, recnum,pid;
	long position;

	if((fd=open(argv[1],O_RDWR))==-1){
		perror(argv[1]);
		exit(1);
	}

	pid=getpid();
	for(;;){
		printf("\nEnter record number: ");
		scanf("%d",&recnum);
		if(recnum<0)
			break;
		position=recnum*sizeof(record);
		lock.l_type=F_WRLCK;
		lock.l_whence=0;
		lock.l_start=position;
		lock.l_len=sizeof(record);
		if(fcntl(fd,F_SETLKW,&lock)==-1){
			perror(argv[1]);
			exit(2);
		}
		lseek(fd,position,0);
		if(read(fd,(char *)&record,sizeof(record))==0){
			printf("record %d not found\n",recnum);
			lock.l_type=F_UNLCK;
			fcntl(fd,F_SETLK,&lock);
			continue;
		}
		printf("Employee: %s, salary: %d\n",record.name,record.salary);
		record.pid=pid;
		printf("Enter new salary: ");
		scanf("%d",&record.salary);
		lseek(fd,position,0);

		write(fd,(char *)&record,sizeof(record));

		lock.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&lock);
	}
	close(fd);
}
                                                                                                                                                                                                                                                                                                                                                                                                                   fdcopy.c                                                                                            0000664 0001750 0001750 00000000611 14244605441 011116  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

#define MSG "message will be written to Terminal\n"

int main(void){
	int new_fd;

	if ((new_fd=fcntl(STDOUT_FILENO, F_DUPFD, 3))== -1){
		fprintf(stderr, "Error : Copying File Descriptor\n");
		exit(1);
	}
	close(STDOUT_FILENO);
	write(3, MSG, strlen(MSG));
	exit(0);
}
                                                                                                                       fddup.c                                                                                             0000664 0001750 0001750 00000000445 14245716567 010756  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

int main(void){
	int testfd;
	int fd;

	fd=open("test.txt",O_CREAT);
	
	testfd=fcntl(fd,F_DUPFD,5);
	printf("testfd :%d\n",testfd);
	testfd=fcntl(fd,F_DUPFD,5);
	printf("testfd :%d\n",testfd);

	getchar();
}

                                                                                                                                                                                                                           kill.c                                                                                              0000664 0001750 0001750 00000000404 14245716130 010564  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int main(void){
	int pid;

	printf("pid : %d\n",getpid());
	
	scanf("%d",&pid);
	if(getpid()==pid)
		kill(pid,SIGKILL);

	
	while(1)
		pause();

	exit(0);
}
                                                                                                                                                                                                                                                            loop.c                                                                                              0000664 0001750 0001750 00000000116 14245716130 010602  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>

int main(void){
	while(1){
		
	}
	printf("not loop\n");
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                  nonblock1.c                                                                                         0000664 0001750 0001750 00000002046 14245731663 011533  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

void set_flags(int fd,int flags);
void clr_flags(int fd,int flags);

char buf[50000];

int main(void){
	int nwrite,ntowrite;
	char *ptr;

	ntowrite=read(STDIN_FILENO,buf,sizeof(buf));
	fprintf(stderr,"reading %d bytes\n",ntowrite);

	set_flags(STDOUT_FILENO,O_NONBLOCK);

	ptr=buf;
	while(ntowrite>0){
		errno=0;
		nwrite=write(STDOUT_FILENO,buf,ntowrite);
		fprintf(stderr,"nwrite=%d, errno=%d\n",nwrite,errno);

		if(nwrite>0){
			ptr+=nwrite;
			ntowrite-=nwrite;
		}
	}
	clr_flags(STDOUT_FILENO,O_NONBLOCK);
}

void set_flags(int fd,int flags){
	int val;

	if((val=fcntl(fd,F_GETFL,0))<0){
		fprintf(stderr,"fcntl error\n");
		exit(1);
	}
	val|=flags;
	if(fcntl(fd,F_SETFL,val)<0){
		fprintf(stderr,"fcntl error\n");
		exit(1);
	}
}

void clr_flags(int fd, int flags){
	int val;

	if((val=fcntl(fd,F_GETFL,0))<0){
		fprintf(stderr,"fcntl error\n");
		exit(1);
	}
	val&=~flags;
	if(fcntl(fd,F_SETFL,val)<0){
		fprintf(stderr,"fcntl error\n");
		exit(1);
	}
}


                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          nonblock2.c                                                                                         0000664 0001750 0001750 00000002066 14245733513 011532  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_CALL 100
#define MAX_BUF 20000

void serror(char *str){
	fprintf(stderr,"%s\n",str);
	exit(1);
}

int main(void){
	int nread,nwrite,val,i=0;
	char *ptr;
	char buf[MAX_BUF];

	int call[MAX_CALL];

	nread=read(STDIN_FILENO,buf,sizeof(buf));
	fprintf(stderr,"read %d bytes\n",nread);

	if((val=fcntl(STDOUT_FILENO,F_GETFL,0))<0)
		serror("fcntl F_GETFL error");
	val|=O_NONBLOCK;
	if(fcntl(STDOUT_FILENO,F_SETFL,val)<0)
		serror("fcntl F_SETFL error");

	for(ptr=buf;nread>0;i++){
		errno=0;
		nwrite=write(STDOUT_FILENO,ptr,nread);

		if(i<MAX_CALL)
			call[i]=nwrite;

		fprintf(stderr,"nwrite = %d, errno = %d\n",nwrite,errno);
		if(nwrite>0){
			ptr+=nwrite;
			nread-=nwrite;
		}
	}

	if((val=fcntl(STDOUT_FILENO,F_GETFL,0))<0)
		serror("fcntl F_GETFL error");
	val &= ~O_NONBLOCK;
	if(fcntl(STDOUT_FILENO,F_SETFL,val)<0)
		serror("fcntl F_SETFL error");

	for(i=0;i<MAX_CALL;i++)
		fprintf(stdout,"call[%d] = %d\n",i, call[i]);

	exit(0);
}

                                                                                                                                                                                                                                                                                                                                                                                                                                                                          open_on_exec.c                                                                                      0000664 0001750 0001750 00000000370 14244665765 012314  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
int main(void)
{
	int fd;
	int val;
	fd = open("exec_copy.txt", O_CREAT);
	execl("/home/chan/Linux/Linux_ex/loop", "./loop", NULL);
	exit(0);
}
                                                                                                                                                                                                                                                                        pthread_cond2.c                                                                                     0000664 0001750 0001750 00000002316 14246125233 012350  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;

int glo_val1=1,glo_val2=2;

void *ssu_thread1(void *arg);
void *ssu_thread2(void *arg);

int main(void){
	pthread_t tid1,tid2;

	pthread_create(&tid1,NULL,ssu_thread1,NULL);
	pthread_create(&tid2,NULL,ssu_thread2,NULL);
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&cond);
	
	exit(0);
}

void *ssu_thread1(void *arg){
	sleep(1);
	glo_val1=2;
	glo_val2=1;

	if(glo_val1>glo_val2)
		pthread_cond_broadcast(&cond);

	printf("ssu_thread1 end\n");
	return NULL;
}


void *ssu_thread2(void *arg){
	struct timespec timeout;
	struct timeval now;

	pthread_mutex_lock(&lock);
	gettimeofday(&now,NULL);

	timeout.tv_sec=now.tv_sec+5;
	timeout.tv_nsec=now.tv_usec*1000;

	if(glo_val1<=glo_val2){
		printf("ssu_thread2 sleep\n");
		if(pthread_cond_timedwait(&cond,&lock,&timeout)==ETIMEDOUT)
			printf("timeout\n");
		else
			printf("glo_val1=%d, glo_val2=%d\n",glo_val1,glo_val2);
	}

	pthread_mutex_unlock(&lock);
	printf("ssu_thread2 end\n");
	return NULL;
}



                                                                                                                                                                                                                                                                                                                  pthread_create2.c                                                                                   0000664 0001750 0001750 00000002447 14246103460 012673  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define THREAD_NUM 8

struct thread_data{
	int thread_index;
	int sum;
	char *message;
};

void *ssu_printhello(void *argv);

struct thread_data thread_data_array[THREAD_NUM];
char *messages[THREAD_NUM];

int main(void){
	pthread_t tid[THREAD_NUM];
	int sum;
	int i;

	sum=0;
	messages[0]="English: hello World!";
	messages[1]="French: hello World!";
	messages[2]="Spanish: hello World!";
	messages[3]="Klingon: hello World!";
	messages[4]="German: hello World!";
	messages[5]="Russia: hello World!";
	messages[6]="Japan: hello World!";
	messages[7]="Latin: hello World!";

	for(i=0;i<THREAD_NUM;i++){
		sum=sum+i;
		thread_data_array[i].thread_index=i;
		thread_data_array[i].sum=sum;
		thread_data_array[i].message=messages[i];
		printf("create thread %d\n",i);

		if(pthread_create(&tid[i],NULL,ssu_printhello,(void *)&thread_data_array[i])!=0){
			fprintf(stderr,"pthread_create error\n");
			exit(1);
		}
	}
	sleep(5);
	exit(0);
}

void *ssu_printhello(void *arg){
	struct thread_data *data;
	char *message;
	int thread_index;
	int sum;

	sleep(1);
	data=(struct thread_data *)arg;
	thread_index=data->thread_index;
	sum=data->sum;
	message=data->message;
	printf("Thread %d:%s   Sum=%d\n",thread_index,message,sum);
	return NULL;
}
                                                                                                                                                                                                                         pthread_join1.c                                                                                     0000664 0001750 0001750 00000001277 14246104615 012371  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *ssu_thread(void *arg);

int main(void){
	pthread_t tid1,tid2;
	int thread1=1;
	int thread2=2;
	int status;

	if(pthread_create(&tid1,NULL,ssu_thread,(void *)&thread1)!=0){
		fprintf(stderr,"pthread_create error\n");
		exit(1);
	}

	if(pthread_create(&tid2,NULL,ssu_thread,(void *)&thread2)!=0){
		fprintf(stderr,"pthread_create error\n");
		exit(1);
	}

	pthread_join(tid1,(void *)&status);
	pthread_join(tid2,(void *)&status);
	exit(0);
}

void *ssu_thread(void *arg){
	int thread_index;
	int i;

	thread_index=*((int *)arg);

	for(i=0;i<5;i++){
		printf("%d : %d\n",thread_index,i);
		sleep(1);
	}
	return NULL;
}
                                                                                                                                                                                                                                                                                                                                 pthread_mutex_lock.c                                                                                0000664 0001750 0001750 00000002263 14246110235 013512  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *ssu_loop1(void *arg);
void *ssu_loop2(void *arg);

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
int shared_value;

int main(void){
	pthread_t tid1,tid2;
	int status;

	shared_value=0;

	if(pthread_create(&tid1,NULL,ssu_loop1,NULL)!=0){
		fprintf(stderr,"error\n");
		exit(1);
	}
	sleep(1);
	if(pthread_create(&tid2,NULL,ssu_loop2,NULL)!=0){
		fprintf(stderr,"error\n");
		exit(1);
	}
	if(pthread_join(tid1,(void *)&status)!=0){
		fprintf(stderr,"error\n");
		exit(1);
	}
	if(pthread_join(tid2,(void *)&status)!=0){
		fprintf(stderr,"error\n");
		exit(1);
	}

	pthread_mutex_destroy(&mutex);
	printf("code = %d\n",shared_value);
	printf("program end\n");
	exit(0);
}

void *ssu_loop1(void *arg){
	int i;

	for(i=0;i<10;i++){
		pthread_mutex_lock(&mutex);
		printf("loop1 : %d\n",shared_value);
		shared_value++;
		if(i==10)
			return NULL;
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
	return NULL;
}


void *ssu_loop2(void *arg){
	int i;

	for(i=0;i<10;i++){
		pthread_mutex_lock(&mutex);
		printf("loop2 : %d\n",shared_value);
		shared_value++;
		pthread_mutex_unlock(&mutex);
		sleep(2);
	}
	return NULL;
}

                                                                                                                                                                                                                                                                                                                                             raise.c                                                                                             0000664 0001750 0001750 00000001172 14245716130 010737  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void ssu_signal_handler1(int signo);
void ssu_signal_handler2(int signo);

int main(void)
{
	if (signal(SIGINT, ssu_signal_handler1) == SIG_ERR) {
		fprintf(stderr, "cannot handle SIGINT\n");
		exit(EXIT_FAILURE);
	}
	if (signal(SIGUSR1, ssu_signal_handler2) == SIG_ERR) {
		fprintf(stderr, "cannot handle SIGUSR1\n");
		exit(EXIT_FAILURE);
	}
	raise(SIGINT);
	raise(SIGUSR1);
	printf("main return\n");
	exit(0);
}

void ssu_signal_handler1(int signo) {
	printf("SIGINT ????????? ??????\n");
}

void ssu_signal_handler2(int signo) {
	printf("SIGUSR1 ????????? ??????\n");
}
                                                                                                                                                                                                                                                                                                                                                                                                      readv.c                                                                                             0000664 0001750 0001750 00000000725 14246032565 010744  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <sys/uio.h>
int main(int argc, char **argv){
	struct iovec vec[2];
	char MSG1[10]={0,};
	char MSG2[10]={0,};
	int str_len;
	vec[0].iov_base=MSG1; //????????? ??????
	vec[0].iov_len=9; // ????????? ????????? ??????.
	vec[1].iov_base=MSG2;
	vec[1].iov_len=9;
	str_len=readv(0, vec, 2); 
	printf("\n??? %d ????????? ??????\n", str_len);
	printf("??? ?????? ????????? : %s \n", MSG1);
	printf("??? ?????? ????????? : %s \n", MSG2);
	return 0;
}
                                           sigaction2.c                                                                                        0000664 0001750 0001750 00000001635 14246061505 011702  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_check_pending(int signo,char *signame);
void ssu_signal_handler(int signo);

int main(void){
	struct sigaction sig_act;
	sigset_t sig_set;

	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags=0;
	sig_act.sa_handler=ssu_signal_handler;

	if(sigaction(SIGUSR1,&sig_act,NULL)!=0){
		fprintf(stderr,"sigaction() error\n");
		exit(1);
	}
	else{
		sigemptyset(&sig_set);
		sigaddset(&sig_set,SIGUSR1);

		if(sigprocmask(SIG_BLOCK,sig_set,NULL)!=0){
			fprintf(stderr,"sigprocmask() error\n");
			exit(1);
		}
		else{
			printf("SIGUSR1 signals ar now blocked\n");
			kill(getpid(),SIGUSR1);
			printf("after kill()\n");
			ssu_check_pending(SIGUSR1,"SIGUSR1");
			sigemptyset(&sig_set);
			sigprocmask(SIG_SETMASK,&sig_set,NULL);
			printf("SIGUSR1 signals are no longer blocked\n");
			ssu_check_pending(SIGUSR1,"SIGUSR1");
		}
	}
	exit(0);
}
                                                                                                   signal2.c                                                                                           0000664 0001750 0001750 00000001454 14245716130 011176  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static void ssu_signal_handler(int signo);

int main(void) {
	if(signal(SIGINT,ssu_signal_handler)==SIG_ERR){
		fprintf(stderr,"cannot handle SIGINT\n");
		exit(EXIT_FAILURE);
	}
	if(signal(SIGTERM,ssu_signal_handler)==SIG_ERR){
		fprintf(stderr,"cannot handle SIGTERM\n");
		exit(EXIT_FAILURE);
	}
	if(signal(SIGHUP,SIG_IGN)==SIG_ERR){
		fprintf(stderr,"ca\n");
		exit(EXIT_FAILURE);
	}
	if(signal(SIGPROF,SIG_DFL)==SIG_ERR){
		fprintf(stderr,"cn\n");
		exit(EXIT_FAILURE);
	}
	while(1)
		pause();
	exit(0);
}


static void ssu_signal_handler(int signo){
	if(signo==SIGINT)
		printf("SIGINT\n");
	else if(signo==SIGTERM)
		printf("SIGTERM\n");
	else{
		fprintf(stderr,"strange\n");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

                                                                                                                                                                                                                    sigpending.c                                                                                        0000664 0001750 0001750 00000000725 14245716130 011766  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
int main(void)
{
	sigset_t pendingset;
	sigset_t sig_set;
	int count = 0;
	sigfillset(&sig_set);
	sigprocmask(SIG_BLOCK, &sig_set, NULL);
	while (1) {
		printf("count: %d\n", count++);
		sleep(1);
		if (sigpending(&pendingset) == 0) {
			if (sigismember(&pendingset, SIGINT)) {
				printf("SIGINT??? ???????????? ?????? ???. ?????? ????????? ??????.\n");
				break;
			}
		}
	}
	exit(0);
}
                                           sigprocmask.c                                                                                       0000664 0001750 0001750 00000000621 14246051033 012147  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
int main(void)
{
	sigset_t sig_set;

	sigemptyset(&sig_set);
	sigaddset(&sig_set,SIGINT);
	sigprocmask(SIG_BLOCK,&sig_set,NULL);

	for(int i=0;i<3;i++){
		printf("%d\n",i);
		sleep(1);
	}
	
	printf("ctrl-c block free\n");
	sigprocmask(SIG_UNBLOCK,&sig_set,NULL);
	printf("not seen if ctrl-c\n");

	while(1);
	
	exit(0);
}
                                                                                                               sigsetjmp3.c                                                                                        0000664 0001750 0001750 00000001641 14246063630 011726  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>
#include <signal.h>

static void ssu_signal_handler1(int signo);
static void ssu_signal_handler2(int signo);

sigjmp_buf jmp_buf1;
sigjmp_buf jmp_buf2;

int main(void){
	struct sigaction act_sig1;
	struct sigaction act_sig2;
	int i,ret;

	printf("My PID is %d\n",getpid());
	ret=sigsetjmp(jmp_buf1,1);

	if(ret==0)
		act_sig1.sa_handler=ssu_signal_handler1;
		sigaction(SIGINT,&act_sig1,NULL);
	}
	else if(ret==3)
		printf("-----------\n");
	
	printf("Starting\n");
	sigsetjmp(jmp_buf2,2);
	act_sig2.sa_handler=ssu_signal_handler2;
	sigaction(SIGUSR1,&act_sig2,NULL);

	for(i=0;i<20;i++){
		printf("i=%d\n",i);
		sleep(1);
	}
	exit(0);
}
	
static void ssu_signal_handler1(int signo){
	fprintf(stderr,"\nInterrupetd\n");
	siglongjmp(jmp_buf1,3);
}
static void ssu_signal_handler2(int signo){
	fprintf(stderr,"\nSIGUSR1\n");
	siglongjmp(jmp_buf1,2);
}
                                                                                               sigsuspend3.c                                                                                       0000664 0001750 0001750 00000001362 14246066756 012121  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

void ssu_signal_handler(int signo);
void ssu_timestamp(char *str);

int main(void) {
	struct sigaction sig_act;
	sigset_t blk_set;

	sigfillset(&blk_set);
	sigdelset(&blk_set, SIGALRM);
	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = 0;
	sig_act.sa_handler = ssu_signal_handler;
	sigaction(SIGALRM, &sig_act, NULL);
	ssu_timestamp("before sigsuspend()");
	alarm(5);
	sigsuspend(&blk_set);
	ssu_timestamp("after sigsuspend()");
	exit(0);
}

void ssu_signal_handler(int signo) {
	printf("in ssu_signal_handler() function\n");
}

void ssu_timestamp(char *str) {
	time_t time_val;
	time(&time_val);
	printf("%s the time is %s\n", str, ctime(&time_val));
}
                                                                                                                                                                                                                                                                              sleep.c                                                                                             0000664 0001750 0001750 00000000535 14246030452 010743  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <unistd.h>

void chan(int signum){
	printf("signal\n");
}

int main(void){
	printf("start\n");
	if(signal(SIGALRM,chan)==SIG_ERR){
		fprintf(stderr,"error\n");
		exit(EXIT_FAILURE);
	}
	alarm(10);
	pause();
	printf("end\n");
	exit(0);
}

                                                                                                                                                                   writev.c                                                                                            0000664 0001750 0001750 00000000676 14246037744 011174  0                                                                                                    ustar   chan                            chan                                                                                                                                                                                                                   #include <stdio.h>
#include <string.h>
#include <sys/uio.h>

int main(int argc, char **argv){
	struct iovec vec[2];
	char MSG1[]="0123456789";
	char MSG2[]="abcdefghijklmnopqrstuvwxyz";
	int str_len;
	vec[0].iov_base=MSG1; //????????? ??????
	vec[0].iov_len=strlen(MSG1); // ????????? ????????? ??????.
	vec[1].iov_base=MSG2;
	vec[1].iov_len=strlen(MSG2);
	str_len=writev(1, vec, 2);
	printf("\n??? %d ????????? ??????\n", str_len);
	return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  