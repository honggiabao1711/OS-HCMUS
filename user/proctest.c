#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    struct procinfo info;
    int target_pid;

    if(argc < 2) target_pid = getpid();
    else target_pid = atoi(argv[1]);

    if(procinfo(target_pid, &info) == 0) {
        printf("PID: %d | PPID: %d | State: %d | Size: %d bytes | Name: %s\n", info.pid, info.ppid, info.state, (int)info.sz, info.name);
    } else {
        printf("Error: Khong tim thay PID %d\n", target_pid);
    }
    
    exit(0);
}