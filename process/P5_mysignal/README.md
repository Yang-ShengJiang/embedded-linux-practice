P5：信号处理（mysignal）

功能：
模拟一个常驻后台的服务进程。通过捕获不同的系统信号来控制程序的运行与退出，实现优雅停机。

编译运行：
gcc -o mysignal mysignal.c
./mysignal

技术点：
- sigaction：使用 sigaction 替代 signal 注册信号处理函数，比 signal 更安全。
- volatile sig_atomic_t：声明全局退出标志 g_running，防止编译器优化，保证异步环境下的读写原子性。
- 异步信号安全：在信号处理函数 handle_usr1 中，使用 write 系统调用替代 printf 打印信息，避免死锁。
- 优雅退出：收到 SIGINT 或 SIGTERM 时，仅修改标志位，由主循环负责执行实际的安全退出和资源清理逻辑。

测试：
- 运行程序后，使用 kill -USR1 <pid>，终端打印状态且程序不退出。
- 按 Ctrl+C 或 kill -TERM <pid>，终端打印清理信息并正常退出。