#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/sysinfo.h"
void
sysinfotest()
{
  struct sysinfo si;

  printf("sysinfotest: starting\n");

  // 1. Gọi system call sysinfo()
  if (sysinfo(&si) < 0) {
    printf("sysinfotest: FAILED\n");
    return;
  }

  // 2. Kiểm tra và in kết quả
  printf("sysinfotest: freemem is %d bytes\n", (int)si.freemem);
  printf("sysinfotest: nproc is %d\n", (int)si.nproc);
  
  // Bạn có thể thêm các kiểm tra logic khác tại đây,
  // ví dụ: kiểm tra xem freemem có > 0 không.
  
  printf("sysinfotest: OK\n");
}

int
main(void)
{
  sysinfotest();
  exit(0);
}