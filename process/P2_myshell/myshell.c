#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

int main(void) {
    char line[MAX_LINE];
    char *args[MAX_ARGS];

    while (1) {
        // 1. 打印提示符
        printf("mysh> ");
        fflush(stdout);

        // 2. 读取一行
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;   // Ctrl+D 或出错
        }

        // 3. 把行末的换行去掉
        int len=strlen(line);
        if(len>0&&line[len-1]=='\n'){
            line[len-1]='\0';
        }

        int i=0;
        // 4. 用 strtok 分割成 args 数组
        char *token=strtok(line," \t\n");
        while(token){
            args[i++]=token;
            token=strtok(NULL," \t\n");
        }
        args[i]=NULL;
        // 注意：args 以 NULL 结尾

        // 5. 如果 args[0] 是 NULL（空行），continue
        if(!args[0]) continue;

        // 6. 如果 args[0] 是 "exit"，break
        if(strcmp(args[0],"exit")==0) break;

        //单独处理cd
        if(strcmp(args[0],"cd")==0){
            const char* target=args[1]?args[1]:getenv("HOME");
            if(chdir(target)==-1){
                perror("cd");
            }
            continue;
        }

        // 7. fork
        pid_t pid = fork();

        // 8. 子进程：execvp，失败打印错误并 exit
        // 9. 父进程：waitpid
        if(pid==-1){
            perror("fork");
        }
        else if(pid==0){
            execvp(args[0],args);
            perror("execvp");
            exit(1);
        }else{
            waitpid(-1,NULL,0);
        }
    }

    return 0;
}