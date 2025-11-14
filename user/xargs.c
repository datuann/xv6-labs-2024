#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"


#define MAX_LINE_LEN 128 

// Hàm thực thi lệnh với đối số được thêm vào
void run_command(char *base_argv[], int base_argc, char *line_arg) {
    if (base_argc + 2 > MAXARG) {
        fprintf(2, "xargs: argument count exceeds MAXARG limit\n");
        return;
    }

    // 1. Khởi tạo mảng đối số cho exec()
    char *child_argv[MAXARG];
    int i;
    // 2. Sao chép lệnh và các đối số ban đầu (base_argv[0] là tên lệnh)
    for (i = 0; i < base_argc; i++) {
        child_argv[i] = base_argv[i];
    }

    // 3. Thêm dòng đọc được từ stdin làm đối số cuối cùng
    child_argv[base_argc] = line_arg;
    
    // 4. Thêm NULL terminator
    child_argv[base_argc + 1] = 0; 

    // 5. Fork và Execute
    int pid = fork();

    if (pid < 0) {
        fprintf(2, "xargs: fork failed\n");
        exit(1);
    }

    if (pid == 0) {  
        exec(child_argv[0], child_argv);
        fprintf(2, "xargs: exec %s failed\n", child_argv[0]);
        exit(1);
    } else {
        wait(0);
    }
}

// Hàm chính đọc từng dòng từ stdin
void read_and_execute(char *base_argv[], int base_argc) {
    char line_buf[MAX_LINE_LEN];
    char *p = line_buf;
    int n;
    
    // Đọc từng ký tự từ stdin (fd 0)
    while ((n = read(0, p, 1)) > 0) {
        
        if (*p == '\n') {
            // Tìm thấy một dòng hoàn chỉnh
            *p = '\0'; // Null-terminate dòng
            
            // Chỉ thực thi nếu dòng không rỗng (sau khi null-terminate)
            if (p > line_buf) {
                run_command(base_argv, base_argc, line_buf);
            }
            
            // Reset buffer pointer cho dòng tiếp theo
            p = line_buf;
        } else if (p - line_buf < MAX_LINE_LEN - 1) {
            p++;
        } else {
            // Dòng quá dài, in cảnh báo, reset và bỏ qua phần còn lại
            fprintf(2, "xargs: line too long, skipping line\n");
            p = line_buf; 
        }
    }
    
    // Xử lý trường hợp input kết thúc mà không có ký tự '\n' cuối cùng
    if (p > line_buf) {
        *p = '\0';
        run_command(base_argv, base_argc, line_buf);
    }
}

void main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs <command> [initial-args...]\n");
        exit(1);
    }

    // Gửi các đối số của lệnh (bắt đầu từ argv[1]) tới hàm đọc và thực thi
    read_and_execute(argv + 1, argc - 1);
    
    exit(0);
}


