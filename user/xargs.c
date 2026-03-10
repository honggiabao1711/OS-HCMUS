#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char* argv[]){
    if(argc < 2){
        fprintf(2,"usage: xargs command\n");
        exit(1);

    }

    char buf[512];   // buffer chua 1 dong  input
    char *cmd_argv[MAXARG]; // argv cho exec
    int i;

    // copy command arguments cua command( bo argv[0] = xargs)
    for( i = 1; i < argc; i++){
        cmd_argv[i-1] = argv[i];
    }

    int base_argc = argc -1; // so argument co dinh ban dau

    int n = 0;  // do dai dong hien tai
    char c;

    while(read(0,&c, 1) > 0){
        if(c == '\n'){
            buf[n] = 0;  // ket thuc chuoi

            cmd_argv[base_argc] = buf; // them dong vao cuoi argv
            cmd_argv[base_argc+1] = 0; // ket thuc mang argv

        

            if(fork() == 0){
                exec(cmd_argv[0], cmd_argv);
                fprintf(2, "exec failed\n");
                exit(1);
            }else{
                wait(0);
            }

            n = 0;  // reset buffer cho dong moi

        }else{
            buf[n++] = c;
        }
    }
    exit(0);
}

