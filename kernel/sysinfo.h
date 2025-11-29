// kernel/sysinfo.h

struct sysinfo {
  uint64 freemem;   // Số byte bộ nhớ trống (free memory) [cite: 98]
  uint64 nproc;     // Số tiến trình đang được sử dụng (processes whose state is not UNUSED) [cite: 98]
  uint64 loadavg;
};