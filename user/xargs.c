#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"


int main(int argc, char *argv[]) {
  char buf[512];
  char *args[MAXARG];
  int i;

  if (argc < 2) {
    fprintf(2, "Usage: xargs command [args...]\n");
    exit(1);
  }

  for (i = 1; i < argc; i++) {
    args[i - 1] = argv[i];
  }

  while (1) {
    int n = 0;
    while (1) {
      char c;
      int r = read(0, &c, 1);
      if (r <= 0) exit(0); 

      if (c == ' ' || c == '\t' || c == '\n') {
        if (n == 0) continue; 
        break; 
      }

      if (n < sizeof(buf) - 1) {
        buf[n++] = c;
      }
    }
    buf[n] = 0; 

    int pid = fork();
    if (pid < 0) {
      exit(1);
    }

    if (pid == 0) {
      args[argc - 1] = buf; 
      args[argc] = 0;      
      exec(args[0], args);
      exit(1);
    } else {
      wait(0); 
    }
  }
  exit(0);
}
