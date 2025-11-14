
#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"


#define MAX_PATH (DIRSIZ + 1)

// Hàm chính thực hiện việc tìm kiếm đệ quy
void find(char *path, char *target_name) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // 1. Mở thư mục
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 2. Lấy thông tin stat của thư mục để kiểm tra loại tệp
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // Chỉ xử lý nếu đây là một thư mục
    if (st.type != T_DIR) {
        char *filename = path + strlen(path);
        while(filename > path && *(filename - 1) != '/') {
            filename--;
        }

        if (strcmp(filename, target_name) == 0) {
             printf("%s\n", path);
        }
        close(fd);
        return;
    }
    
    // Đảm bảo buffer có đủ không gian cho đường dẫn mới (bao gồm '/')
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
        fprintf(2, "find: path too long\n");
        close(fd);
        return;
    }
    
    // Copy đường dẫn hiện tại vào buffer
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/'; 
    
    // 3. Đọc từng mục nhập trong thư mục
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
            continue;
        
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            continue;
        
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0; 
        
        // 4. Lấy stat của mục nhập (sử dụng đường dẫn đầy đủ trong buf)
        if (stat(buf, &st) < 0) {
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }

        if (st.type == T_DIR) {
            // 5a. Nếu là thư mục, gọi đệ quy
            find(buf, target_name);
            
        } else if (st.type == T_FILE) {
            // 5b. Nếu là file, so sánh tên
            if (strcmp(de.name, target_name) == 0) {
                printf("%s\n", buf); 
            }
        }
    }

    // 6. Đóng file descriptor
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find <directory> <filename>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    
    exit(0);
}
