#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *name);

int main(int argc, char *argv[]) {
  if (argc <= 2) {
    fprintf(2, "Usage: path and file name \n");
    exit(1);
  }
  char *path = argv[1];
  char *name = argv[2];
  find(path, name);
  exit(0);
}

void find(char *path, char *name) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
    case T_FILE:
      fprintf(2, "find: path must be a directory\n");
      break;

    case T_DIR:
      if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
        printf("find: path too long\n");
        break;
      }
      strcpy(buf, path);
      p = buf + strlen(buf);
      *p++ = '/'; // 在末尾加上斜杠，并将指针 p 往后移动一格
      while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
          continue;
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
          continue;

        // 将当前的目录名/文件名拷贝到 p 指向的位置
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        if (stat(buf, &st) < 0) {
          printf("find: cannot stat %s\n", buf);
          continue;
        }

        if (st.type == T_FILE) {
          if (strcmp(de.name, name) == 0) {
            printf("%s\n", buf); 
          }
        } else if (st.type == T_DIR) {
          find(buf, name); // 递归深入
        }
      }
      break;
  }
  close(fd);
}
