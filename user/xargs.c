#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  char *args[MAXARG];
  char buf[512];
  int i;

  // 1. Lưu các đối số cố định (ví dụ: echo bye) vào mảng args
  for (i = 1; i < argc; i++) {
    args[i - 1] = argv[i];
  }

  // 2. Đọc dữ liệu từ stdin (đường ống pipe)
  while (1) {
    int n = 0;
    while (1) {
      // Đọc từng ký tự từ stdin cho đến khi gặp dấu xuống dòng
      int r = read(0, &buf[n], 1); 
      if (r <= 0 || buf[n] == '\n') break; 
      n++;
    }

    if (n == 0) break; // Hết dữ liệu thì thoát vòng lặp
    buf[n] = 0; // Kết thúc chuỗi bằng ký tự null

    // 3. Đưa chuỗi vừa đọc được vào cuối danh sách đối số
    args[argc - 1] = buf;
    args[argc] = 0; // Đánh dấu kết thúc mảng đối số cho hàm exec

    // 4. Tạo tiến trình con để chạy lệnh
    if (fork() == 0) {
      exec(args[0], args); 
      exit(0);
    } else {
      wait(0); // Cha đợi con xong rồi mới đọc dòng tiếp theo
    }
  }
  exit(0);
}