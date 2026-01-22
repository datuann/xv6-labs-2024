#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


void primes(int p_read) __attribute__((noreturn));

// Hàm Sàng (Sieve)
// Nhận vào đầu đọc (read end) của pipe từ tiến trình bên trái.
void primes(int p_read) {
    int prime; 
    int n;     
    int p_new[2]; // Pipe mới để truyền cho tiến trình Sàng tiếp theo

    // 1. Đọc số đầu tiên. Đây là số nguyên tố mới (prime).
    if (read(p_read, &prime, sizeof(int)) != sizeof(int)) {
        close(p_read);
        exit(0); 
    }
    printf("prime %d\n", prime);
    
    // 2. Tạo pipe mới cho tiến trình bên phải
    if (pipe(p_new) < 0) {
        fprintf(2, "primes: pipe failed\n");
        close(p_read);
        exit(1); 
    }
    
    // 3. Tạo tiến trình con (tiến trình Sàng tiếp theo)
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "primes: fork failed\n");
        close(p_read);
        close(p_new[0]);
        close(p_new[1]);
        exit(1); 
    }
    
    if (pid > 0) { 
        // Tiến trình Cha (Tiến trình Sàng hiện tại)
        
        close(p_new[0]);
        
        // 4. Lọc các số còn lại từ pipe đầu vào
        while (read(p_read, &n, sizeof(int)) == sizeof(int)) {
            // Nếu n KHÔNG chia hết cho prime, truyền nó sang pipe bên phải
            if (n % prime != 0) {
                if (write(p_new[1], &n, sizeof(int)) != sizeof(int)) {
                    break; 
                }
            }
        }
        

        close(p_read);  
        close(p_new[1]); 
        
        // Chờ tiến trình con (tiến trình Sàng tiếp theo) kết thúc
        wait(0);
        
    } else {
        // Tiến trình Con (Tiến trình Sàng tiếp theo)
   
        close(p_new[1]);
        close(p_read);

        primes(p_new[0]);
    }
    

    exit(0);
}

int main(int argc, char *argv[]) {
    int p[2]; // Pipe đầu tiên (Nguồn -> Sàng 2)
    
    // 1. Tạo pipe đầu tiên
    if (pipe(p) < 0) {
        fprintf(2, "primes: pipe failed\n");
        exit(1); 
    }
    // 2. Tạo tiến trình Sàng đầu tiên
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "primes: fork failed\n");
        exit(1); 
    }
    
    if (pid > 0) { 
        // Tiến trình Cha (Tiến trình Nguồn - Source)
        
       
        close(p[0]);
        
        // 3. Đẩy các số từ 2 đến 280 vào pipeline
        for (int i = 2; i <= 300; i++) {
            if (write(p[1], &i, sizeof(int)) != sizeof(int)) {
                fprintf(2, "primes: source write failed at %d\n", i);
                break;
            }
        }
        
        // 4. ĐÓNG đầu ghi -> báo hiệu EOF cho tiến trình Sàng đầu tiên
        close(p[1]);
        
        // 5. Chờ toàn bộ pipeline (tất cả các tiến trình con, cháu...) kết thúc
        wait(0);
        
    } else {
        // Tiến trình Con (Tiến trình Sàng đầu tiên)
        
        close(p[1]);
        primes(p[0]);
    }
    
    // Tiến trình chính kết thúc
    exit(0);
}