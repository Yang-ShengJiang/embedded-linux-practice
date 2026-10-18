#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>

int main(void){
    int pipefd[2];
    char buf[1024];
    pid_t pid;

    if(pipe(pipefd)==-1){
        perror("pipe");
        exit(1);
    }

    pid=fork();
    if(pid==-1){
        perror("fork");
        exit(1);
    }
    else if(pid==0){
        close(pipefd[0]);
        char *msg="Hello";
        if(write(pipefd[1],msg,strlen(msg))==-1){
            perror("write");
            _exit(1);
        };
        close(pipefd[1]);
        _exit(0);
    }else{
        close(pipefd[1]);
        int n=read(pipefd[0],buf,sizeof(buf));
        if(n==-1){
            perror("read");
            exit(1);
        }
        buf[n]='\0';
        printf("父进程收到:%s\n",buf);

        close(pipefd[0]);
        wait(NULL);
    }
    return 0;
}