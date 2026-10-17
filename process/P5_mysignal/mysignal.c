#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

// 定义一个全局退出标志，主循环依赖它判断是否继续
volatile sig_atomic_t g_running = 1;

// 结束运行辅助函数
void handle_exit(int signo) {
    g_running=0;
}

// SIGUSR1辅助函数
void handle_usr1(int signo) {
    const char*msg ="收到状态查询请求，当前正在运行...";
    write(STDOUT_FILENO,msg,strlen(msg));
}

// 实现sigaction安全注册逻辑
void setup_signal(int signo, void (*handler)(int)) {
    struct sigaction sa;
    sa.sa_handler=handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=0;

    if(sigaction(signo,&sa,NULL)==-1){
        perror("sigaction");
        exit(1);
    }
}

int main(void) {
    // 1. 注册 SIGINT 和 SIGTERM 到 handle_exit
    setup_signal(SIGINT,handle_exit);
    setup_signal(SIGTERM,handle_exit);
    // 2. 注册 SIGUSR1 到 handle_usr1
    setup_signal(SIGUSR1,handle_usr1);
    // 3. 打印程序启动信息（包含 PID）
    printf("服务已启动，PID:%d\n",getpid());
    printf("按 Ctrl+C 退出，或运行 kill -USR1 %d 查看状态\n",getpid());
    // 4. 主循环 (依赖全局标志)
    while (g_running) {
        sleep(1);
    }

    // 5. 循环结束，执行清理逻辑
    printf("收到退出信号，正在清理资源...\n");
    sleep(2);
    printf("已成功退出\n");

    return 0;
}