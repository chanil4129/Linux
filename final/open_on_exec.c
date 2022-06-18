#include <stdlib.h>
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
