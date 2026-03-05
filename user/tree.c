#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "user/user.h"

char* fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Tìm tên file cuối cùng sau '/'
  for(p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  if(strlen(p) >= DIRSIZ)
    return p;

  memmove(buf, p, strlen(p));
  buf[strlen(p)] = 0;
  return buf;
}

void tree(char *path, int depth)
{
  char buf[512], *p;
  int fd;
  struct stat st;
  struct dirent de;

  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "tree: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "tree: cannot stat %s\n", path);
    close(fd);
    return;
  }

  // In indentation
  for(int i = 0; i < depth; i++)
    printf("  ");

  if(st.type == T_DIR)
    printf("%s/\n", fmtname(path));
  else {
    printf("%s\n", fmtname(path));
    close(fd);
    return;
  }

  if(st.type != T_DIR){
    close(fd);
    return;
  }

  if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
    printf("tree: path too long\n");
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';

  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)
      continue;

    if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
      continue;

    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    tree(buf, depth + 1);
  }

  close(fd);
}

int main(int argc, char *argv[])
{
  if(argc > 2){
    fprintf(2, "usage: tree [directory]\n");
    exit(1);
  }

  if(argc == 1)
    tree(".", 0);
  else
    tree(argv[1], 0);

  exit(0);
}
