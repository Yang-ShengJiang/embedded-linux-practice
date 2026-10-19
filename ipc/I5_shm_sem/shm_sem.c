#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <string.h>

#define SHM_NAME "/count_shm"
#define SEM_NAME "/mutex_sem"

// 1. 定义共享内存里的数据结构（只需要一个 int count）
// 你的代码：
typedef struct {
    int count;
}SharedCounter;


// 2. 定义全局指针和描述符（方便统一清理）
int g_shm_fd = -1;
sem_t *g_sem = SEM_FAILED;
// 你的代码：定义 SharedCounter 类型的全局指针 counter，初始化为 MAP_FAILED
SharedCounter *counter=MAP_FAILED;

// 3. 注册的清理函数（仿照 I4 写）
void cleanup(void){
    // 提示：清理 counter（munmap）、g_sem（sem_close/sem_unlink）、g_shm_fd（close/shm_unlink）
    // 你的代码：
    if(counter!=MAP_FAILED){
        munmap(counter,sizeof(SharedCounter));
        counter=MAP_FAILED;
    }
    if(g_sem!=SEM_FAILED){
        sem_close(g_sem);
        sem_unlink(SEM_NAME);
        g_sem=SEM_FAILED;
    }
    if(g_shm_fd!=-1){
        close(g_shm_fd);
        shm_unlink(SHM_NAME);
        g_shm_fd=-1;
    }
}

int main(void){
    atexit(cleanup);
    
    // 防呆：删除可能残留的文件
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_NAME);

    // 4. 创建共享内存并设置大小
    // 你的代码：
    g_shm_fd=shm_open(SHM_NAME,O_RDWR|O_CREAT,0666);
    if(g_shm_fd==-1){
        perror("shm_open");
        exit(1);
    }
    if(ftruncate(g_shm_fd,sizeof(SharedCounter))==-1){
        perror("ftruncate");
        exit(1);
    }
    // 5. 映射共享内存，强转为结构体指针，并用 memset 清零
    // 你的代码：
    counter=(SharedCounter*)mmap(NULL,sizeof(SharedCounter),PROT_WRITE|PROT_READ,MAP_SHARED,g_shm_fd,0);
    if(counter==MAP_FAILED){
        perror("mmap");
        exit(1);
    }
    memset(counter,0,sizeof(SharedCounter));
    
    // 6. 创建信号量，初始值必须设为 1 (代表互斥锁)
    // 你的代码：
    g_sem=sem_open(SEM_NAME,O_CREAT,0666,1);

    // ---------- 阶段二：三军混战 ----------
    // 循环 3 次 fork 出 3 个子进程
    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        
        if (pid == 0) {
            // ========== 子进程逻辑 ==========
            // 循环 10000 次：
            for (int j = 0; j < 10000; j++) {
                // 1. 加锁 (sem_wait)
                sem_wait(g_sem);
                // 2. 临界区：读取 count，加 1，写回
                // 你的代码：
                counter->count++;
                // 3. 解锁 (sem_post)
                sem_post(g_sem);
            }
            _exit(0); // 子进程干完活，直接退出
        }
    }

    // ========== 父进程逻辑 ==========
    // 父进程也参与加 10000 次的操作
    for (int j = 0; j < 10000; j++) {
        // 同样的加锁、修改、解锁
        // 你的代码：
        sem_wait(g_sem);
        counter->count++;
        sem_post(g_sem);
    }

    // 7. 父进程必须等待所有 3 个子进程结束
    // 提示：循环 3 次调用 wait()
    // 你的代码：
    for(int i=0;i<3;i++){
        wait(NULL);
    }

    // 8. 打印最终的 count 值
    // 提示：正确结果应该等于 40000
    // 你的代码：
    printf("%d\n",counter->count);

    return 0;
}