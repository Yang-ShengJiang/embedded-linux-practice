I1：POSIX 共享内存通信

功能：
父子进程通过 POSIX 共享内存 (shm_open + mmap) 交换字符串。使用 POSIX 有名信号量 (sem_open) 实现同步：父进程写完数据后 V 操作唤醒子进程，子进程 P 操作等待读取。

技术点：
- shm_open + ftruncate + mmap：创建并映射共享内存。
- sem_open + sem_wait + sem_post：创建信号量并实现 P/V 操作。
- 进程同步：初始信号量为 0，保证子进程必须等待父进程写入。
- 工程化清理：使用 atexit 注册统一的 cleanup 函数，结合全局变量和“释放后置空”机制，避免资源泄漏和二次释放。
- 区分 exit 与 _exit：子进程使用 _exit 退出，避免错误触发父进程注册的清理函数。

编译运行：
gcc -o shm_test shm_test.c -lrt -pthread
./shm_test