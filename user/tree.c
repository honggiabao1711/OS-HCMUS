#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// Mảng đánh dấu: tầng nào đã hết mục con thì không vẽ vạch dọc nữa
int is_last[20]; 

void print_indent(int level) {
  for (int i = 0; i < level; i++) {
    if (i == level - 1) {
      printf(is_last[i] ? "└── " : "├── ");
    } else {
      printf(is_last[i] ? "    " : "│   ");
    }
  }
}

char* fmtname(char *path) {
  char *p;
  for(p=path+strlen(path); p >= path && *p != '/'; p--);
  p++;
  return p;
}

void tree(char *path, int level) {
  char buf[512], *p;
  int fd, fd2;
  struct dirent de;
  struct stat st;
  int count = 0, current = 0;

  if((fd = open(path, 0)) < 0) return;
  if(fstat(fd, &st) < 0 || st.type != T_DIR) { close(fd); return; }

  // Bước 1: Đếm xem thư mục này có bao nhiêu mục con
  fd2 = open(path, 0);
  while(read(fd2, &de, sizeof(de)) == sizeof(de)) {
    if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
    count++;
  }
  close(fd2);

  // Bước 2: Duyệt và in
  strcpy(buf, path);
  p = buf + strlen(buf);
  if(p > buf && *(p-1) != '/') *p++ = '/';

  while(read(fd, &de, sizeof(de)) == sizeof(de)) {
    if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
    
    current++;
    is_last[level] = (current == count); // Đánh dấu nếu là mục cuối cùng

    print_indent(level + 1);
    printf("%s\n", de.name);

    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    tree(buf, level + 1);
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  char *path = (argc < 2) ? "." : argv[1];
  printf("%s\n", path);
  tree(path, 0);
  exit(0);
}