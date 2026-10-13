#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<errno.h>

#define BUF_SIZE 4096

int main(int argc, char const *argv[])
{
    if(argc!=3){
        fprintf(stderr,"用法：%s <源文件> <目标文件>\n",argv[0]);
        return 1;
    }
    char *src_path=argv[1];
    char *dst_path=argv[2];
    int src_fd=open(src_path,O_RDONLY);
    if(src_fd==-1){
        fprintf(stderr,"错误；源文件打开失败 %s:%s\n",src_path,strerror(errno));
        return 1;
    }
    int dst_fd=open(dst_path,O_WRONLY|O_CREAT|O_TRUNC,0664);
    if(src_fd==-1){
        fprintf(stderr,"错误；目标文件打开失败 %s:%s\n",dst_path,strerror(errno));
        close(src_fd);
        return 1;
    }
    char *buf[BUF_SIZE];
    ssize_t n_read;
    ssize_t total;

    while((n_read=read(src_fd,buf,BUF_SIZE))>0){
        ssize_t n_written=0;
        while(n_written<n_read){
            ssize_t w=(dst_fd,buf+n_written,n_read-n_written);
            if(w==-1){
                fprintf(stderr,"错误：写入失败：%s",strerror(errno));
                close(src_fd);
                close(dst_fd);
                return 1;
            }
            n_written+=w;
        }
        total+=n_written;
    }

    if(n_read==-1){
        fprintf(stderr,"错误：读取失败：%s",strerror(errno));
        close(src_fd);
        close(dst_fd);
        return 1;
    }

    printf("拷贝成功，共拷贝%s个字符\n",total);
    return 0;
}
