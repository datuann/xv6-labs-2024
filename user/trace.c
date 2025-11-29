#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // XÓA DÒNG NÀY (Dòng 8):
  // int i; 
  
  int mask;

  if(argc < 3){
    fprintf(2, "usage: trace mask command [args...]\n");
    exit(1);
  }

  // 1. Lấy mask từ đối số đầu tiên (argv[1])
  mask = atoi(argv[1]);

  // 2. Gọi system call trace() đã được triển khai trong kernel
  if (trace(mask) < 0) {
    fprintf(2, "trace: failed to set mask\n");
    exit(1);
  }

  // 3. Thực thi chương trình con (argv[2])
  exec(argv[2], argv + 2);

  // Lệnh này chỉ chạy nếu exec thất bại
  fprintf(2, "exec %s failed\n", argv[2]);
  exit(1);
}