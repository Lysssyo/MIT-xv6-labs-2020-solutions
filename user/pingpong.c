#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
	char buf1[1];
	char buf2[1];
	int p1[2]; // 管道1，父 -> 子
	int p2[2]; // 管道2，子 -> 父
	pipe(p1);
	pipe(p2);
	int pid = fork();
	if (pid == 0){
		read(p1[0], buf2, sizeof buf2);
		printf("%d: received ping\n", getpid());
		write(p2[1], buf2, 1);
		exit(0);
	} else {
		write(p1[1], "a", 1);
		wait(0);
		read(p2[0], buf1, sizeof buf1);
		printf("%d: received pong\n", getpid());
		exit(0);
	}
}