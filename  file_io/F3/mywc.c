#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "用法: %s <文件>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "错误: 无法打开 %s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    char buf[BUF_SIZE];
    ssize_t n_read;

    long lines = 0;      // 行数
    long words = 0;      // 单词数
    long chars = 0;      // 字符数
    int in_word = 0;     // 状态：是否在单词内 0不在 1在

    while ((n_read = read(fd, buf, BUF_SIZE)) > 0) {
        for (int i = 0; i < n_read; i++) {
            char c = buf[i];
            // 状态机逻辑
            if(c==' '||c=='\n'||c=='\t'){
                if(c=='\n'){
                    lines++;
                }
                if(in_word){
                    in_word=0;
                    words++;
                }
            }
            else{
                in_word=1;
            }
            chars++;
        }
    }

    // 循环结束后，检查最后一个单词
    if(in_word==1){
        words++;
    }

    close(fd);

    printf("行数: %ld  单词数: %ld  字符数: %ld\n", lines, words, chars);
    return 0;
}