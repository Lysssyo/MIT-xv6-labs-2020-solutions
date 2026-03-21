#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 每个进程的处理函数
// p: 从左邻居传来的管道，p[0]是读端，p[1]是写端
void perProcess(int *p) {
  // 这个进程只需要读左邻居的数据，关闭写端
  close(p[1]);

  int num;
  // 读第一个数，即当前级别的质数
  // 如果读到0，说明左邻居已经没有数据了，直接退出
  if (read(p[0], &num, sizeof(int)) == 0) {
    close(p[0]);
    exit(0);
  }
  printf("prime %d\n", num);
  int first = num; // 记录当前质数，用于过滤倍数

  // 创建新管道，连接右邻居
  int p2[2];
  pipe(p2);

  if (fork() == 0) {
    // 子进程：成为右邻居，递归处理下一级
    // 不需要左邻居的管道了
    close(p[0]);
    perProcess(p2); // 递归，处理下一级质数
  } else {
    // 父进程：过滤数据，把不是first倍数的数写给右邻居
    close(p2[0]); // 父进程不需要p2的读端

    while (read(p[0], &num, sizeof(int)) > 0) {
      if (num % first != 0) {
        // 不是first的倍数，传给右邻居继续筛
        write(p2[1], &num, sizeof(int));
      }
    }
    // 数据写完，关闭写端，右邻居的read会返回0
    close(p2[1]);
    close(p[0]);
    wait(0);  // 等待右邻居（子进程）退出
    exit(0);
  }
}

int main(int argc, char *argv[]) {
  // 创建第一个管道，作为流水线入口
  int p1[2];
  pipe(p1);

  if (fork() == 0) {
    // 子进程：启动流水线，处理第一级质数
    perProcess(p1);
  } else {
    // 父进程：生产数据，把2~35写入管道
    close(p1[0]); // 父进程不需要读端
    for (int i = 2; i <= 35; i++) {
      write(p1[1], &i, sizeof(int));
    }
    close(p1[1]); // 写完关闭写端
    wait(0);      // 等待子进程（整条流水线）结束
  }
  exit(0);
}