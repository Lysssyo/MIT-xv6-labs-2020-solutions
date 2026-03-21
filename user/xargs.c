#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
  char buf[512];
  char *xargv[MAXARG];
  int i;

  // 1. 复制固定参数
  for (i = 1; i < argc; i++) {
    xargv[i - 1] = argv[i];
  }

  int curr = 0;
  // 2. 循环读取标准输入
  while (read(0, &buf[curr], 1) > 0) {
    if (buf[curr] == '\n') {
      buf[curr] = '\0'; // 换行符替换为结束符

      if (fork() == 0) {
        // 核心：在子进程中处理这一行的拆分
        int xargc = argc - 1; // 从固定参数之后开始填
        char *p = buf;
        for (int j = 0; j <= curr; j++) {
          if (buf[j] == ' ' || buf[j] == '\0') {
            buf[j] = '\0';
            if (p != &buf[j]) { // 如果不是空单词
              xargv[xargc++] = p;
            }
            p = &buf[j + 1];
          }
        }
        xargv[xargc] = 0; // 封口
        exec(xargv[0], xargv);
        exit(0);
      } else {
        wait(0);
        curr = 0; // 重置缓冲区
      }
    } else {
      curr++;
    }
  }
  exit(0);
}