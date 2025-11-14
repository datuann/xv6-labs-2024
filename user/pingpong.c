#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Chương trình pingpong sử dụng hai pipe để trao đổi dữ liệu
// Pipe 1 (p1): Parent -> Child
// Pipe 2 (p2): Child -> Parent
void
main(int argc, char *argv[])
{
  int p1[2]; 
  int p2[2]; 
  
  char buf[1]; // Buffer 1 byte để trao đổi dữ liệu
  
  // 1. Tạo hai pipe
  if (pipe(p1) < 0) {
    fprintf(2, "pingpong: pipe 1 failed\n");
    exit(1);
  }
  if (pipe(p2) < 0) {
    fprintf(2, "pingpong: pipe 2 failed\n");
    exit(1);
  }

  // 2. Fork để tạo tiến trình con
  int pid = fork();

  if (pid < 0) {
    fprintf(2, "pingpong: fork failed\n");
    exit(1);
  }

  if (pid > 0) {
    // TIẾN TRÌNH CHA (PARENT)


    close(p1[0]); 
    close(p2[1]); 

    // 1. Cha gửi 1 byte ('p' - ping) cho con
    if (write(p1[1], "p", 1) != 1) {
      fprintf(2, "pingpong: parent write failed\n");
      exit(1);
    }
    close(p1[1]); 

    // 2. Cha chờ và đọc 1 byte từ con
    if (read(p2[0], buf, 1) != 1) {
      fprintf(2, "pingpong: parent read failed\n");
      exit(1);
    }
    close(p2[0]); 

    // 3. In thông báo "received pong"
    printf("%d: received pong\n", getpid());
    
    // 4. Chờ con kết thúc
    wait((int *)0);
    
  } else {
    // --- TIẾN TRÌNH CON (CHILD) ---

    close(p1[1]); 
    close(p2[0]); 

    // 1. Con chờ và đọc 1 byte từ cha
    if (read(p1[0], buf, 1) != 1) {
      fprintf(2, "pingpong: child read failed\n");
      exit(1);
    }
    close(p1[0]); 

    // 2. In thông báo "received ping"
    printf("%d: received ping\n", getpid());

    // 3. Con gửi lại 1 byte (bất kỳ) cho cha
    if (write(p2[1], buf, 1) != 1) {
      fprintf(2, "pingpong: child write failed\n");
      exit(1);
    }
    close(p2[1]); 
    
    // 4. Con kết thúc
    exit(0);
  }

  exit(0);
}